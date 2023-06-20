package it.unisa.dia.gas.plaf.jpbc.pairing.d;

import it.unisa.dia.gas.jpbc.*;
import it.unisa.dia.gas.plaf.jpbc.field.curve.CurveElement;
import it.unisa.dia.gas.plaf.jpbc.field.curve.CurveField;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteElement;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteField;
import it.unisa.dia.gas.plaf.jpbc.field.poly.PolyModElement;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingMap;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingPreProcessing;
import it.unisa.dia.gas.plaf.jpbc.util.math.BigIntegerUtils;

import java.util.List;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class TypeDTateAffineNoDenomMillerPairingMap extends AbstractMillerPairingMap<Polynomial> {
    protected TypeDPairing pairing;

    protected int pairingPreProcessingTableLength = -1;
    protected int pairingPreProcessingLengthInBytes = -1;


    public TypeDTateAffineNoDenomMillerPairingMap(TypeDPairing pairing) {
        super(pairing);

        this.pairing = pairing;
    }


    public Element pairing(Point in1, Point in2) {
        // map from twist: (x, y) --> (v^-1 x, v^-(3/2) y)
        // where v is the quadratic non-residue used to construct the twist
        Polynomial Qx = (Polynomial) in2.getX().duplicate().mul(pairing.nqrInverse);
        // v^-3/2 = v^-2 * v^1/2
        Polynomial Qy = (Polynomial) in2.getY().duplicate().mul(pairing.nqrInverseSquare);

        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), tatePow(pairing(in1, Qx, Qy)));
    }

    public boolean isProductPairingSupported() {
        return true;
    }

    public Element pairing(Element[] in1, Element[] in2) {
        int n = in1.length;

        CurveElement[] Qs = new CurveElement[n];

        for (int i = 0; i < n; i++) {
            Point Q = (Point) in2[i];

            Qs[i] = (CurveElement) Q.getField().newElement();
            // map from twist: (x, y) --> (v^-1 x, v^-(3/2) y)
            // where v is the quadratic non-residue used to construct the twist
            Qs[i].getX().set(Q.getX()).mul(pairing.nqrInverse);
            // v^-3/2 = v^-2 * v^1/2
            Qs[i].getY().set(Q.getY()).mul(pairing.nqrInverseSquare);
        }

        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), tatePow(pairingInternal(in1, Qs)));
    }

    public void finalPow(Element element) {
        element.set(tatePow(element));
    }

    public int getPairingPreProcessingLengthInBytes() {
        if (pairingPreProcessingLengthInBytes == -1){
            pairingPreProcessingTableLength = pairing.r.bitLength()  + BigIntegerUtils.hammingWeight(pairing.r) - 1;
            pairingPreProcessingLengthInBytes = 4 + (pairingPreProcessingTableLength * 3 * pairing.Fq.getLengthInBytes());
        }

        return pairingPreProcessingLengthInBytes;
    }

    public PairingPreProcessing pairing(Point in1) {
        return new TypeDMillerNoDenomAffinePairingPreProcessing(in1);
    }

    public PairingPreProcessing pairing(byte[] source, int offset) {
        return new TypeDMillerNoDenomAffinePairingPreProcessing(source, offset);
    }

    public boolean isAlmostCoddh(Element a, Element b, Element c, Element d) {
        // Twist: (x, y) --> (v^-1 x, v^-(3/2) y)
        // where v is the quadratic nonresidue used to construct the twist.

        Element cx = ((Point) c).getX().duplicate().mul(pairing.nqrInverse);
        Element dx = ((Point) d).getX().duplicate().mul(pairing.nqrInverse);

        // v^-3/2 = v^-2 * v^1/2
        Element cy = ((Point) c).getY().duplicate().mul(pairing.nqrInverseSquare);
        Element dy = ((Point) d).getY().duplicate().mul(pairing.nqrInverseSquare);

        Element t0 = pairing((Point) a, (Polynomial) dx, (Polynomial) dy);
        Element t1 = pairing((Point) b, (Polynomial) cx, (Polynomial) cy);
        t0 = tatePow(t0);
        t1 = tatePow(t1);
        Element t2 = t0.duplicate().mul(t1);

        if (t2.isOne())
            return true;    // We were given g, g^x, h, h^-x.
        else {
            // Cheaply check the other case.
            t2.set(t0).mul(t1.invert());
            if (t2.isOne())
                return true;    // We were given g, g^x, h, h^x.
        }

        return false;
    }


    public Element tatePow(Element element) {
        if (pairing.k == 6) {
            Point<Polynomial> e0, e3;
            PolyModElement e2;

            e0 = pairing.Fqk.newElement();
            e2 = pairing.Fqd.newElement();
            e3 = pairing.Fqk.newElement();

            Polynomial e0re = e0.getX();
            Polynomial e0im = e0.getY();

            Element e0re0 = e0re.getCoefficient(0);
            Element e0im0 = e0im.getCoefficient(0);

            Point<Polynomial> in = (Point<Polynomial>) element;
            List<Element> inre = in.getX().getCoefficients();
            List<Element> inmi = in.getY().getCoefficients();

            qPower(1, e2, e0re, e0im, e0re0, e0im0, inre, inmi);
            e3.set(e0);
            e0re.set(in.getX());
            e0im.set(in.getY()).negate();
            e3.mul(e0);
            qPower(-1, e2, e0re, e0im, e0re0, e0im0, inre, inmi);
            e0.mul(in);
            e0.invert();
            in.set(e3).mul(e0);
            e0.set(in);

            return lucasEven(e0, pairing.phikOnr);
        } else {
            return element.duplicate().pow(pairing.tateExp);
        }
    }

    final void qPower(int sign, PolyModElement e2,
                      Element e0re, Element e0im, Element e0re0, Element e0im0,
                      List<Element> inre, List<Element> inim) {
        e2.set(pairing.xPowq).polymodConstMul(inre.get(1));
        e0re.set(e2);
        e2.set(pairing.xPowq2).polymodConstMul(inre.get(2));
        e0re.add(e2);
        e0re0.add(inre.get(0));

        if (sign > 0) {
            e2.set(pairing.xPowq).polymodConstMul(inim.get(1));
            e0im.set(e2);
            e2.set(pairing.xPowq2).polymodConstMul(inim.get(2));
            e0im.add(e2);
            e0im0.add(inim.get(0));
        } else {
            e2.set(pairing.xPowq).polymodConstMul(inim.get(1));
            e0im.set(e2).negate();
            e2.set(pairing.xPowq2).polymodConstMul(inim.get(2));
            e0im.sub(e2);
            e0im0.sub(inim.get(0));
        }
    }

    protected Element pairing(Point P, Polynomial Qx, Polynomial Qy) {
        Element Px = P.getX();
        Element Py = P.getY();

        Point Z = (Point) P.duplicate();
        Element Zx = Z.getX();
        Element Zy = Z.getY();

        Element a = Px.getField().newElement();
        Element b = a.duplicate();
        Element c = a.duplicate();
        Element cca = ((CurveField) P.getField()).getA();
        Element temp = a.duplicate();

        Point<Polynomial> f0 = pairing.Fqk.newElement();
        Element f = pairing.Fqk.newOneElement();

        for (int m = pairing.r.bitLength() - 2; m > 0; m--) {
            tangentStep(f0, a, b, c, Zx, Zy, cca, temp, Qx, Qy, f);
            Z.twice();

            if (pairing.r.testBit(m)) {
                lineStep(f0, a, b, c, Zx, Zy, Px, Py, temp, Qx, Qy, f);
                Z.add(P);
            }

            f.square();
        }
        tangentStep(f0, a, b, c, Zx, Zy, cca, temp, Qx, Qy, f);

        return f;
    }

    protected Element pairingInternal(Element[] Ps, Element[] Qs) {
        Field refField = Ps[0].getField();

        Element[] Zs = new Element[Ps.length];
        for (int i = 0; i < Zs.length; i++) {
            Zs[i] = Ps[i].duplicate();
        }

        Element a = ((Point) Ps[0]).getX().getField().newElement();
        Element b = a.duplicate();
        Element c = a.duplicate();
        Element cca = ((CurveField) Ps[0].getField()).getA();
        Element temp = a.duplicate();

        Point<Polynomial> f0 = pairing.Fqk.newElement();
        Element f = pairing.Fqk.newOneElement();

        for (int m = pairing.r.bitLength() - 2; m > 0; m--) {
            tangentStep(f0, a, b, c, Zs, cca, temp, Qs, f);
            refField.twice(Zs);

            if (pairing.r.testBit(m)) {
                lineStep(f0, a, b, c, Zs, Ps, temp, Qs, f);
                refField.add(Zs, Ps);
            }

            f.square();
        }
        tangentStep(f0, a, b, c, Zs, cca, temp, Qs, f);

        return f;
    }


    protected void millerStep(Point<Polynomial> out, Element a, Element b, Element c, Polynomial Qx, Polynomial Qy) {
        // a, b, c are in Fq
        // point Q is (Qx, Qy * sqrt(nqr)) where nqr is used to construct
        // the quadratic field extension Fqk of Fqd

        Polynomial rePart = out.getX();
        Polynomial imPart = out.getY();
        for (int i = 0, d = rePart.getDegree(); i < d; i++) {
            rePart.getCoefficient(i).set(Qx.getCoefficient(i)).mul(a);
            imPart.getCoefficient(i).set(Qy.getCoefficient(i)).mul(b);
        }
        rePart.getCoefficient(0).add(c);
    }


    public int getPairingPreProcessingTableLength() {
        getPairingPreProcessingLengthInBytes();
        return pairingPreProcessingTableLength;
    }

    public class TypeDMillerNoDenomAffinePairingPreProcessing extends AbstractMillerPairingPreProcessing {

        public TypeDMillerNoDenomAffinePairingPreProcessing(byte[] source, int offset) {
            super(pairing, source, offset);
        }

        public TypeDMillerNoDenomAffinePairingPreProcessing(Point in1) {
            super(in1, getPairingPreProcessingTableLength());

            Element Px = in1.getX();
            Element Py = in1.getY();

            Point Z = (Point) in1.duplicate();
            Element Zx = Z.getX();
            Element Zy = Z.getY();

            Element a = pairing.Fq.newElement();
            Element b = pairing.Fq.newElement();
            Element c = pairing.Fq.newElement();
            Element curveA = ((CurveField) in1.getField()).getA();
            Element temp = pairing.Fq.newElement();

            for (int m = pairing.r.bitLength() - 2; m > 0; m--) {
                computeTangent(processingInfo, a, b, c, Zx, Zy, curveA, temp);
                Z.twice();

                if (pairing.r.testBit(m)) {
                    computeLine(processingInfo, a, b, c, Zx, Zy, Px, Py, temp);
                    Z.add(in1);
                }
            }
            computeTangent(processingInfo, a, b, c, Zx, Zy, curveA, temp);
        }

        public Element pairing(Element in2) {
            Point pointIn2 = (Point) in2;

            // map from twist: (x, y) --> (v^-1 x, v^-(3/2) y)
            // where v is the quadratic non-residue used to construct the twist
            Polynomial Qx = (Polynomial) pointIn2.getX().duplicate().mul(pairing.nqrInverse);
            // v^-3/2 = v^-2 * v^1/2
            Polynomial Qy = (Polynomial) pointIn2.getY().duplicate().mul(pairing.nqrInverseSquare);

            Point<Polynomial> f0 = pairing.Fqk.newElement();
            Element out = pairing.Fqk.newOneElement();
            int row = 0;

            for (int m = pairing.r.bitLength() - 2; m > 0; m--) {
                millerStep(f0, processingInfo.table[row][0], processingInfo.table[row][1], processingInfo.table[row][2], Qx, Qy);
                out.mul(f0);
                row++;

                if (pairing.r.testBit(m)) {
                    millerStep(f0, processingInfo.table[row][0], processingInfo.table[row][1], processingInfo.table[row][2], Qx, Qy);
                    out.mul(f0);
                    row++;
                }

                out.square();
            }
            millerStep(f0, processingInfo.table[row][0], processingInfo.table[row][1], processingInfo.table[row][2], Qx, Qy);
            out.mul(f0);

            return new GTFiniteElement(
                    TypeDTateAffineNoDenomMillerPairingMap.this,
                    (GTFiniteField) pairing.getGT(),
                    tatePow(out)
            );
        }
    }

}

package it.unisa.dia.gas.plaf.jpbc.pairing.a;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.PairingPreProcessing;
import it.unisa.dia.gas.jpbc.Point;
import it.unisa.dia.gas.plaf.jpbc.field.curve.CurveElement;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteElement;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteField;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingMap;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingPreProcessing;

import java.math.BigInteger;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class TypeATateProjectiveMillerPairingMap extends AbstractMillerPairingMap {
    protected TypeAPairing pairing;

    protected int pairingPreProcessingTableLength = -1;
    protected int pairingPreProcessingLenghtInBytes = -1;


    public TypeATateProjectiveMillerPairingMap(TypeAPairing pairing) {
        super(pairing);

        this.pairing = pairing;
    }


    /**
     * in1, in2 are from E(F_q), out from F_q^2
     */
    public Element pairing(Point in1, Point in2) {
        // could save a couple of inversions by avoiding
        // this function and rewriting lineStep() to handle projective coords
        // convert V from weighted projective (Jacobian) to affine
        // i.e. (X, Y, Z) --> (X/Z^2, Y/Z^3)
        // also sets z to 1

        Point V = (Point) in1.duplicate();
        Element Vx = V.getX();
        Element Vy = V.getY();
        Element z = pairing.Fq.newOneElement();
        Element z2 = pairing.Fq.newOneElement();

        Element Qx = in2.getX();
        Element Qy = in2.getY();

        // The coefficients of the line equation
        Element a = pairing.Fq.newElement();
        Element b = pairing.Fq.newElement();
        Element c = pairing.Fq.newElement();

        Point f0 = pairing.Fq2.newElement();
        Point f = pairing.Fq2.newOneElement();

        // Temp element
        Element e0 = pairing.Fq.newElement();

        // Remember that r = 2^exp2 + sign1 * 2^exp1 + sign0 * 1 (Solinas prime)

        int i = 0;
        int n = pairing.exp1;
        for (; i < n; i++) {
            // f = f^2 g_V,V(Q)
            // where g_V,V = tangent at V
            f.square();

            tangentStepProjective(f0, a, b, c, Vx, Vy, z, z2, e0, Qx, Qy, f);
            twiceProjective(e0, a, b, c, Vx, Vy, z, z2);
        }

        // Move to affine
        pointToAffine(Vx, Vy, z, z2, e0);

        Element f1;
        Point V1 = pairing.Eq.newElement();
        if (pairing.sign1 < 0) {
            V1.set(V).negate();
            f1 = f.duplicate().invert();
        } else {
            V1.set(V);
            f1 = f.duplicate();
        }

        n = pairing.exp2;
        for (; i < n; i++) {
            f.square();

            tangentStepProjective(f0, a, b, c, Vx, Vy, z, z2, e0, Qx, Qy, f);
            twiceProjective(e0, a, b, c, Vx, Vy, z, z2);
        }

        f.mul(f1);

        // Move to affine again
        pointToAffine(Vx, Vy, z, z2, e0);

        lineStep(f0, a, b, c, Vx, Vy, V1.getX(), V1.getY(), e0, Qx, Qy, f);

        // Do final pow
        Point out = pairing.Fq2.newElement();
        tatePow(out, f, f0, pairing.phikOnr);

        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), out);
    }

    @Override
    public int getPairingPreProcessingLengthInBytes() {
        if (pairingPreProcessingLenghtInBytes == -1){
            pairingPreProcessingTableLength = pairing.exp2 + 1;
            pairingPreProcessingLenghtInBytes = 4 + (pairingPreProcessingTableLength * 3 * pairing.Fq.getLengthInBytes());
        }

        return pairingPreProcessingLenghtInBytes;
    }

    public void finalPow(Element element) {
        Element t0 = element.getField().newElement();
        tatePow((Point) t0, (Point) element, pairing.phikOnr);
        element.set(t0);
    }

    public PairingPreProcessing pairing(Point in1) {
        return new TypeATateProjectiveMillerPairingPreProcessing(in1);
    }

    public boolean isProductPairingSupported() {
        return true;
    }

    public Element pairing(Element[] in1, Element[] in2) {
        // could save a couple of inversions by avoiding
        // this function and rewriting lineStep() to handle projective coords
        // convert V from weighted projective (Jacobian) to affine
        // i.e. (X, Y, Z) --> (X/Z^2, Y/Z^3)
        // also sets z to 1

        Field refField = in1[0].getField();

        CurveElement[] Vs = new CurveElement[in1.length];
        CurveElement[] V1s = new CurveElement[in1.length];

        for(int i=0; i< in1.length; i++){
            Vs[i] = (CurveElement) in1[i].duplicate();
            V1s[i] = (CurveElement) in1[i].getField().newElement();
        }


        // The coefficients of the line equation
        Element a = pairing.Fq.newElement();
        Element b = pairing.Fq.newElement();
        Element c = pairing.Fq.newElement();
        Element curveA = pairing.Fq.newOneElement();

        Point f0 = pairing.Fq2.newElement();
        Point f = pairing.Fq2.newOneElement();

        // Temp element
        Element e0 = pairing.Fq.newElement();

        // Remember that r = 2^exp2 + sign1 * 2^exp1 + sign0 * 1 (Solinas prime)
        int i = 0;
        int n = pairing.exp1;
        for (; i < n; i++) {
            // f = f^2 g_V,V(Q)
            // where g_V,V = tangent at V
            f.square();
            tangentStep(f0, a, b, c, Vs, curveA, e0, in2, f);
            refField.twice(Vs);
        }

        Element f1;
        if (pairing.sign1 < 0) {
            for (int j = 0; j < V1s.length; j++) {
                V1s[j].set(Vs[j]).negate();
            }
            f1 = f.duplicate().invert();
        } else {
            for (int j = 0; j < V1s.length; j++) {
                V1s[j].set(Vs[j]);
            }
            f1 = f.duplicate();
        }

        n = pairing.exp2;
        for (; i < n; i++) {
            f.square();
            tangentStep(f0, a, b, c, Vs, curveA, e0, in2, f);
            refField.twice(Vs);
        }

        f.mul(f1);

        lineStep(f0, a, b, c, Vs, V1s, e0, in2, f);

        // Do final pow
        Point out = pairing.Fq2.newElement();
        tatePow(out, f, f0, pairing.phikOnr);

        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), out);
    }

    public PairingPreProcessing pairing(byte[] source, int offset) {
        return new TypeATateProjectiveMillerPairingPreProcessing(source, offset);
    }

    protected final void millerStep(Point out, Element a, Element b, Element c, Element Qx, Element Qy) {
        // we will map Q via (x,y) --> (-x, iy)
        // hence:
        // Re(a Qx + b Qy + c) = -a Q'x + c and
        // Im(a Qx + b Qy + c) = b Q'y

        Element rePart = out.getX();
        Element imPart = out.getY();

        rePart.set(c).sub(imPart.set(a).mul(Qx));
        imPart.set(b).mul(Qy);
    }


    final void tatePow(Point out, Point in, Point temp, BigInteger cofactor) {
        Element in1 = in.getY();
        //simpler but slower:
        //element_pow_mpz(out, f, tateExp);

        //1. Exponentiate by q-1
        //which is equivalent to the following

        temp.set(in).invert();
        in1.negate();
        in.mul(temp);

        //2. Exponentiate by (q+1)/r

        //Instead of:
        //	element_pow_mpz(out, in, cofactor);
        //we use Lucas sequences (see "Compressed Pairings", Scott and Barreto)
        lucasOdd(out, in, temp, cofactor);
    }

    final void tatePow(Point out, Point in, BigInteger cofactor) {
        Element in1 = in.getY();
        //simpler but slower:
        //element_pow_mpz(out, f, tateExp);

        //1. Exponentiate by q-1
        //which is equivalent to the following

        Point temp = (Point) in.duplicate().invert();
        in1.negate();
        in.mul(temp);

        //2. Exponentiate by (q+1)/r

        //Instead of:
        //	element_pow_mpz(out, in, cofactor);
        //we use Lucas sequences (see "Compressed Pairings", Scott and Barreto)
        lucasOdd(out, in, temp, cofactor);
    }

    final void twiceProjective(Element e0, Element a, Element b, Element c, Element Vx, Element Vy, Element z, Element z2) {
        // e0 = 3x^2 + cca z^4  (cca = 1)
        e0.set(Vx).square().add(a.set(e0).twice()).add(a.set(z2).square());

        // z = 2 y z
        z.mul(Vy).twice();
        z2.set(z).square();

        //a = 4 x y^2
        b.set(Vy).square();
        a.set(Vx).mul(b).twice().twice();

        // x_out = e0^2 - 2 a
        c.set(a).twice();
        Vx.set(e0).square().sub(c);

        //b = 8y^4
        b.square().twice().twice().twice();

        //y_out = e0(a - x_out) - b
        a.sub(Vx);
        e0.mul(a);
        Vy.set(e0).sub(b);
    }


    public int getPairingPreProcessingTableLength() {
       getPairingPreProcessingLengthInBytes();
       return pairingPreProcessingTableLength;
    }

    public class TypeATateProjectiveMillerPairingPreProcessing extends AbstractMillerPairingPreProcessing {

        public TypeATateProjectiveMillerPairingPreProcessing(byte[] source, int offset) {
            super(pairing, source, offset);
        }

        public TypeATateProjectiveMillerPairingPreProcessing(Point in1) {
            super(in1, getPairingPreProcessingTableLength());

            Point V = (Point) in1.duplicate();
            Point V1 = pairing.Eq.newElement();

            Element Vx = V.getX();
            Element Vy = V.getY();

            Element V1x = V1.getX();
            Element V1y = V1.getY();

            Element a = pairing.Fq.newElement();
            Element b = pairing.Fq.newElement();
            Element c = pairing.Fq.newElement();
            Element curveA = pairing.Fq.newOneElement();
            Element temp = pairing.Fq.newElement();

            int n = pairing.exp1, i;
            for (i = 0; i < n; i++) {
                computeTangent(processingInfo, a, b, c, Vx, Vy, curveA, temp);
                V.twice();
            }

            if (pairing.sign1 < 0) {
                V1.set(V).negate();
            } else {
                V1.set(V);
            }

            n = pairing.exp2;
            for (; i < n; i++) {
                computeTangent(processingInfo, a, b, c, Vx, Vy, curveA, temp);
                V.twice();
            }

            computeLine(processingInfo, a, b, c, Vx, Vy, V1x, V1y, temp);
        }

        public Element pairing(Element in2) {
            //TODO: use proj coords here too to shave off a little time
            Point pointIn2 = (Point) in2;

            Element Qx = pointIn2.getX();
            Element Qy = pointIn2.getY();
            int i, n;
            Point f0 = pairing.Fq2.newElement();
            Point f = pairing.Fq2.newOneElement();
            Point out = pairing.Fq2.newElement();

            for (i = 0, n = pairing.exp1; i < n; i++) {
                f.square();
                millerStep(f0, processingInfo.table[i][0], processingInfo.table[i][1], processingInfo.table[i][2], Qx, Qy);
                f.mul(f0);
            }

            if (pairing.sign1 < 0) {
                out.set(f).invert();
            } else {
                out.set(f);
            }

            for (n = pairing.exp2; i < n; i++) {
                f.square();
                millerStep(f0, processingInfo.table[i][0], processingInfo.table[i][1], processingInfo.table[i][2], Qx, Qy);
                f.mul(f0);
            }

            f.mul(out);
            millerStep(f0, processingInfo.table[i][0], processingInfo.table[i][1], processingInfo.table[i][2], Qx, Qy);
            f.mul(f0);

            tatePow(out, f, f0, pairing.phikOnr);

            return new GTFiniteElement(
                    TypeATateProjectiveMillerPairingMap.this,
                    (GTFiniteField) pairing.getGT(),
                    out
            );
        }
    }

}

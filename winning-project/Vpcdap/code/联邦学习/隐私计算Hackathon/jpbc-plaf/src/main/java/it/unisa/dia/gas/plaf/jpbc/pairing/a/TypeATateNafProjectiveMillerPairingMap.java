package it.unisa.dia.gas.plaf.jpbc.pairing.a;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.PairingPreProcessing;
import it.unisa.dia.gas.jpbc.Point;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteElement;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteField;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingMap;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingPreProcessing;
import it.unisa.dia.gas.plaf.jpbc.util.math.BigIntegerUtils;

import java.math.BigInteger;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class TypeATateNafProjectiveMillerPairingMap extends AbstractMillerPairingMap {
    protected final TypeAPairing pairing;
    protected final byte[] r;
    
    protected int pairingPreProcessingTableLength = -1;
    protected int pairingPreProcessingLengthInBytes = -1;


    public TypeATateNafProjectiveMillerPairingMap(TypeAPairing pairing) {
        super(pairing);

        this.pairing = pairing;
        this.r = BigIntegerUtils.naf(pairing.r, (byte) 2);
    }

    /**
     * in1, in2 are from E(F_q), out from F_q^2
     */
    public Element pairing(Point P, Point Q) {
        Point f = pairing.Fq2.newOneElement();
        Point u = pairing.Fq2.newElement();

        JacobPoint V = new JacobPoint(P.getX(), P.getY(), P.getX().getField().newOneElement());
        Point nP = (Point) P.duplicate().negate();

        Element a = pairing.Fq.newElement();
        Element b = pairing.Fq.newElement();
        Element c = pairing.Fq.newElement();

        for (int i = r.length - 2; i >= 0; i--) {
            twice(V, a, b, c);

            millerStep(u, a, b, c, Q.getX(), Q.getY());
            f.square().mul(u);

            switch (r[i]) {
                case 1:
                    add(V, P, a, b, c);

                    millerStep(u, a, b, c, Q.getX(), Q.getY());
                    f.mul(u);
                    break;
                case -1:
                    add(V, nP, a, b, c);

                    millerStep(u, a, b, c, Q.getX(), Q.getY());
                    f.mul(u);
                    break;
            }
        }

        Point out = pairing.Fq2.newElement();
        tatePow(out, f, pairing.phikOnr);
        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), out);
    }

    public void finalPow(Element element) {
        Element t0 = element.getField().newElement();
        tatePow((Point) t0, (Point) element, pairing.phikOnr);
        element.set(t0);
    }

    public int getPairingPreProcessingLengthInBytes() {
        if (pairingPreProcessingLengthInBytes == -1){
            pairingPreProcessingTableLength = r.length - 1 + BigIntegerUtils.hammingWeight(r, r.length - 2);
            pairingPreProcessingLengthInBytes = 4 + (pairingPreProcessingTableLength * 3 * pairing.Fq.getLengthInBytes());
        }

        return pairingPreProcessingLengthInBytes;
    }

    public PairingPreProcessing pairing(Point in1) {
        return new TypeATateNafProjectiveMillerPairingPreProcessing(in1);
    }

    public PairingPreProcessing pairing(byte[] source, int offset) {
        return new TypeATateNafProjectiveMillerPairingPreProcessing(source, offset);
    }


    protected final void millerStep(Point out, Element a, Element b, Element c, Element Qx, Element Qy) {
        out.getX().set(c).add(a.duplicate().mul((Qx)));
        out.getY().set(b).mul(Qy);
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

    /**
     * point doubling in Jacobian coordinates
     */
    final void twice(JacobPoint V, Element a, Element b, Element c) {
        //if(V.isInfinity())
        //	return;

        Element x = V.getX();
        Element y = V.getY();
        Element z = V.getZ();

        //t1 = y^2
        Element t1 = y.duplicate().square();

        //t2 = 4 x t1 = 4 x y^2
        Element t2 = x.duplicate().mul(t1).twice().twice();

        //t4 = z^2
        b.set(z).square();

        //t5 = 3 x^2 + a t4^2 = 3 x^2 + a z^4
        a.set(x).square().mul(3).add(b.duplicate().square());
        c.set(a).mul(x).sub(t1).sub(t1);

        //x3 = (3 x^2 + a z^4)^2 - 2 (4 x y^2)

        //z3 = 2 y z
        z.mul(y).twice();

        //y3 = 3 x^2 + a z^4 (4 x y^2 - x3) - 8 y^4
        V.setX(a.duplicate().square().sub(t2.duplicate().twice()));
        V.setY(a.duplicate().mul(t2.duplicate().sub(V.getX())).sub(t1.duplicate().square().twice().twice().twice()));

        a.mul(b);
        b.mul(z);
    }

    /**
     * add two point, save result in the first argument
     */
    final void add(JacobPoint V, Point P, Element a, Element b, Element c) {
        Element x1 = V.getX();
        Element y1 = V.getY();
        Element z1 = V.getZ();

        Element x = P.getX();
        Element y = P.getY();

        //t1=z1^2
        Element t1 = z1.duplicate().square();
        //t2=z1t1
        Element t2 = z1.duplicate().mul(t1);
        //t3=xt1
        Element t3 = x.duplicate().mul(t1);
        //t4=Yt2
        Element t4 = y.duplicate().mul(t2);
        //t5=t3-x1
        Element t5 = t3.duplicate().sub(x1);
        //t6=t4-y1
        Element t6 = t4.duplicate().sub(y1);
        //t7=t5^2
        Element t7 = t5.duplicate().square();
        //t8=t5t7
        Element t8 = t5.duplicate().mul(t7);
        //t9=x1t7
        Element t9 = x1.duplicate().mul(t7);

        //x3=t6^2-(t8+2t9)
        Element x3 = t6.duplicate().square().sub(t8.duplicate().add(t9.duplicate().twice()));

        //y3=t6(t9-x3)-y1t8
        Element y3 = t6.duplicate().mul(t9.duplicate().sub(x3)).sub((y1.duplicate().mul(t8)));

        //z3=z1t5
        Element z3 = z1.duplicate().mul(t5);

        V.setX(x3);
        V.setY(y3);
        V.setZ(z3);

        a.set(t6);
        b.set(z3);
        c.set(t6).mul(x).sub(z3.duplicate().mul(y));

        //(z3 Q.y)i -(z3 y - t6 (Q.x + x))
//        u.getX().set(Q.getX().duplicate().add(x).mul(t6).sub(z3.duplicate().mul(y)));
//        u.getY().set(z3.duplicate().mul(Q.getY()));
    }


    public int getPairingPreProcessingTableLength() {
       getPairingPreProcessingLengthInBytes();
       return pairingPreProcessingTableLength;
    }


    public class TypeATateNafProjectiveMillerPairingPreProcessing extends AbstractMillerPairingPreProcessing {

        public TypeATateNafProjectiveMillerPairingPreProcessing(byte[] source, int offset) {
            super(pairing, source, offset);
        }

        public TypeATateNafProjectiveMillerPairingPreProcessing(Point in1) {
            super(in1, getPairingPreProcessingTableLength());

            JacobPoint V = new JacobPoint(in1.getX(), in1.getY(), in1.getX().getField().newOneElement());
            Point nP = (Point) in1.duplicate().negate();

            Element a = pairing.Fq.newElement();
            Element b = pairing.Fq.newElement();
            Element c = pairing.Fq.newElement();

            for (int i = r.length - 2; i >= 0; i--) {
                twice(V, a, b, c);
                processingInfo.addRow(a, b, c);

                switch (r[i]) {
                    case 1:
                        add(V, in1, a, b, c);
                        processingInfo.addRow(a, b, c);

                        break;
                    case -1:
                        add(V, nP, a, b, c);
                        processingInfo.addRow(a, b, c);

                        break;
                }
            }
        }

        public Element pairing(Element in2) {
            Point Q = (Point) in2;
            Point f = pairing.Fq2.newOneElement();
            Point u = pairing.Fq2.newElement();

            for (int i = r.length - 2, coeffIndex = 0; i >= 0; i--) {
                millerStep(u, processingInfo.table[coeffIndex][0], processingInfo.table[coeffIndex][1], processingInfo.table[coeffIndex][2], Q.getX(), Q.getY());
                f.square().mul(u);

                coeffIndex++;

                switch (r[i]) {
                    case 1:
                        millerStep(u, processingInfo.table[coeffIndex][0], processingInfo.table[coeffIndex][1], processingInfo.table[coeffIndex][2], Q.getX(), Q.getY());
                        f.mul(u);

                        coeffIndex++;
                        break;
                    case -1:
                        millerStep(u, processingInfo.table[coeffIndex][0], processingInfo.table[coeffIndex][1], processingInfo.table[coeffIndex][2], Q.getX(), Q.getY());
                        f.mul(u);

                        coeffIndex++;
                        break;
                }
            }

            Point out = pairing.Fq2.newElement();
            tatePow(out, f, pairing.phikOnr);
            return new GTFiniteElement(TypeATateNafProjectiveMillerPairingMap.this, (GTFiniteField) pairing.getGT(), out);
        }
    }

}

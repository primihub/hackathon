package it.unisa.dia.gas.plaf.jpbc.pairing.a1;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.Point;
import it.unisa.dia.gas.plaf.jpbc.field.curve.CurveElement;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteElement;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteField;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.AbstractMillerPairingMap;

import java.math.BigInteger;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class TypeA1TateAffineMillerPairingMap extends AbstractMillerPairingMap<Element> {
    protected TypeA1Pairing pairing;


    public TypeA1TateAffineMillerPairingMap(TypeA1Pairing pairing) {
        super(pairing);

        this.pairing = pairing;
    }


    public Element pairing(Point in1, Point in2) {
        Element Px = in1.getX();
        Element Py = in1.getY();

        Element Qx = in2.getX();
        Element Qy = in2.getY();

        Point V = (Point) in1.duplicate();
        Element Vx = V.getX();
        Element Vy = V.getY();

        Element a = pairing.Fp.newElement();
        Element b = pairing.Fp.newElement();
        Element c = pairing.Fp.newElement();
        Element curveA = pairing.Fp.newOneElement();
        Element e0 = pairing.Fp.newElement();

        Point<Element> f0 = pairing.Fq2.newElement();
        Point out = pairing.Fq2.newElement();
        Point f = pairing.Fq2.newOneElement();

        for (int m = pairing.r.bitLength() - 2; m > 0; m--) {
            tangentStep(f0, a, b, c, Vx, Vy, curveA, e0, Qx, Qy, f);
            V.twice();

            if (pairing.r.testBit(m)) {
                lineStep(f0, a, b, c, Vx, Vy, Px, Py, e0, Qx, Qy, f);
                V.add(in1);
            }

            f.square();
        }
        tangentStep(f0, a, b, c, Vx, Vy, curveA, e0, Qx, Qy, f);

        // Tate exponentiation.
        // Simpler but slower:
        //   element_pow_mpz(out, f, p->tateExp);
        // Use this trick instead:
        f0.set(f).invert();
        f.getY().negate();
        f.mul(f0);
        out.set(f).pow(pairing.phikOnr);

        /* We could use this instead but p->h is small so this does not help much
        a_tateexp(out, f, f0, p->h);
        */

        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), out);
    }

    public boolean isProductPairingSupported() {
        return true;
    }

    public Element pairing(Element[] in1, Element[] in2) {
        Field refField = in1[0].getField();

        CurveElement[] Vs = new CurveElement[in1.length];

        for (int i = 0; i < in1.length; i++) {
            Vs[i] = (CurveElement) in1[i].duplicate();
        }


        Element a = pairing.Fp.newElement();
        Element b = pairing.Fp.newElement();
        Element c = pairing.Fp.newElement();
        Element curveA = pairing.Fp.newOneElement();
        Element e0 = pairing.Fp.newElement();

        Point<Element> f0 = pairing.Fq2.newElement();
        Point out = pairing.Fq2.newElement();
        Point f = pairing.Fq2.newOneElement();

        for (int m = pairing.r.bitLength() - 2; m > 0; m--) {
            tangentStep(f0, a, b, c, Vs, curveA, e0, in2, f);

            refField.twice(Vs);

            if (pairing.r.testBit(m)) {
                lineStep(f0, a, b, c, Vs, in1, e0, in2, f);
                refField.add(Vs, in1);
            }

            f.square();
        }
        tangentStep(f0, a, b, c, Vs, curveA, e0, in2, f);

        // Tate exponentiation.
        // Simpler but slower:
        //   element_pow_mpz(out, f, p->tateExp);
        // Use this trick instead:
        f0.set(f).invert();
        f.getY().negate();
        f.mul(f0);
        out.set(f).pow(pairing.phikOnr);

        /* We could use this instead but p->h is small so this does not help much
        a_tateexp(out, f, f0, p->h);
        */

        return new GTFiniteElement(this, (GTFiniteField) pairing.getGT(), out);
    }

    public void finalPow(Element element) {
        Element t0, t1;
        t0 = element.getField().newElement();
        t1 = element.getField().newElement();

        tatePow((Point) t0, (Point) element, (Point) t1, pairing.phikOnr);

        element.set(t0);
    }


    public Element tatePow(Element element) {
        Element t0, t1;
        t0 = element.getField().newElement();
        t1 = element.getField().newElement();

        tatePow((Point) t0, (Point) element, (Point) t1, pairing.phikOnr);

        element.set(t0);

        return element;
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

/*        element_invert(temp, in);
        element_neg(in1, in1);
        element_mul(in, in, temp);
  */
        //2. Exponentiate by (q+1)/r

        //Instead of:
        //	element_pow_mpz(out, in, cofactor);
        //we use Lucas sequences (see "Compressed Pairings", Scott and Barreto)
        lucasOdd(out, in, temp, cofactor);
    }


    protected void millerStep(Point out, Element a, Element b, Element c, Element Qx, Element Qy) {
        // we will map Q via (x,y) --> (-x, iy)
        // hence:
        // Re(a Qx + b Qy + c) = -a Q'x + c and
        // Im(a Qx + b Qy + c) = b Q'y

        Element rePart = out.getX();
        Element imPart = out.getY();

        rePart.set(c).sub(imPart.set(a).mul(Qx));
        imPart.set(b).mul(Qy);
    }

}

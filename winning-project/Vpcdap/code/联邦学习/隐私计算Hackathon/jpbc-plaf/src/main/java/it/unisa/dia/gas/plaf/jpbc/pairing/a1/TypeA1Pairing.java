package it.unisa.dia.gas.plaf.jpbc.pairing.a1;

import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.PairingParameters;
import it.unisa.dia.gas.jpbc.Point;
import it.unisa.dia.gas.plaf.jpbc.field.curve.CurveField;
import it.unisa.dia.gas.plaf.jpbc.field.gt.GTFiniteField;
import it.unisa.dia.gas.plaf.jpbc.field.quadratic.DegreeTwoExtensionQuadraticField;
import it.unisa.dia.gas.plaf.jpbc.field.z.ZrField;
import it.unisa.dia.gas.plaf.jpbc.pairing.AbstractPairing;

import java.math.BigInteger;
import java.security.SecureRandom;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class TypeA1Pairing extends AbstractPairing {
    public static final String NAF_MILLER_PROJECTTIVE_METHOD = "naf-miller-projective";
    public static final String MILLER_AFFINE_METHOD = "miller-affine";


    protected BigInteger r;
    protected BigInteger p;
    protected long l;

    protected BigInteger phikOnr;

    protected Field Fp;
    protected Field<? extends Point> Fq2;
    protected Field<? extends Point> Eq;


    public TypeA1Pairing(PairingParameters params) {
        this(new SecureRandom(), params);
    }

    public TypeA1Pairing(SecureRandom random, PairingParameters params) {
        super(random);

        initParams(params);
        initMap(params);
        initFields();
    }

    protected void initParams(PairingParameters curveParams) {
        // validate the type
        String type = curveParams.getString("type");
        if (type == null || !"a1".equalsIgnoreCase(type))
            throw new IllegalArgumentException("Type not valid. Found '" + type + "'. Expected 'a1'.");

        // load params
        p = curveParams.getBigInteger("p");
        r = curveParams.getBigInteger("n");
        l = curveParams.getLong("l");
    }


    protected void initFields() {
        // Init Zr
        Zr = initFp(r);

        // Init Fp
        Fp = initFp(p);

        //k=2, hence phi_k(q) = q + 1, phikOnr = (q+1)/r
        phikOnr = BigInteger.valueOf(l);

        // Init Eq
        Eq = initEq();

        // Init Fq2
        Fq2 = initFi();

        // Init G1, G2, GT
        G1 = Eq;
        G2 = G1;
        GT = initGT();
    }


    protected Field initFp(BigInteger order) {
        return new ZrField(random, order);
    }

    protected Field<? extends Point> initEq() {
        return new CurveField<Field>(random, Fp.newOneElement(), Fp.newZeroElement(), r, phikOnr);
    }

    protected Field<? extends Point> initFi() {
        return new DegreeTwoExtensionQuadraticField<Field>(random, Fp);
    }

    protected Field initGT() {
        return new GTFiniteField(random, r, pairingMap, Fq2);
    }

    protected void initMap(PairingParameters curveParams) {
        String method = curveParams.getString("method", NAF_MILLER_PROJECTTIVE_METHOD);

        if (NAF_MILLER_PROJECTTIVE_METHOD.endsWith(method)) {
            pairingMap = new TypeA1TateNafProjectiveMillerPairingMap(this);
        } else if (MILLER_AFFINE_METHOD.equals(method))
            pairingMap = new TypeA1TateAffineMillerPairingMap(this);
        else
            throw new IllegalArgumentException("Pairing method not recognized. Method = " + method);
    }

}
package it.unisa.dia.gas.plaf.jpbc.pairing.product;

import it.unisa.dia.gas.jpbc.*;
import it.unisa.dia.gas.plaf.jpbc.field.vector.VectorField;
import it.unisa.dia.gas.plaf.jpbc.pairing.accumulator.PairingAccumulator;
import it.unisa.dia.gas.plaf.jpbc.pairing.accumulator.PairingAccumulatorFactory;
import it.unisa.dia.gas.plaf.jpbc.pairing.accumulator.ProductPairingAccumulator;
import it.unisa.dia.gas.plaf.jpbc.pairing.map.DefaultPairingPreProcessing;

import java.security.SecureRandom;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class ProductPairing implements Pairing {
    protected Pairing basePairing;
    protected Field G1, G2;

    public ProductPairing(SecureRandom random, Pairing basePairing, int n) {
        this.basePairing = basePairing;

        this.G1 = new VectorField(random, basePairing.getG1(), n);
        this.G2 = new VectorField(random, basePairing.getG2(), n);
    }


    public boolean isSymmetric() {
        return basePairing.isSymmetric();
    }

    public Field getG1() {
        return G1;
    }

    public Field getG2() {
        return G2;
    }

    public Field getGT() {
        return basePairing.getGT();
    }

    public Field getZr() {
        return basePairing.getZr();
    }

    public int getDegree() {
        return 2;
    }

    public Field getFieldAt(int index) {
        switch (index) {
            case 0:
                return basePairing.getZr();
            case 1:
                return G1;
            case 2:
                return G2;
            case 3:
                return basePairing.getGT();
            default:
                throw new IllegalArgumentException("invalid index");
        }
    }

    public Element pairing(Element in1, Element in2) {
        Vector v1 = (Vector) in1;
        Vector v2 = (Vector) in2;

        PairingAccumulator combiner = (basePairing.isProductPairingSupported()) ?
                new ProductPairingAccumulator(basePairing, v1.getSize()) :
                PairingAccumulatorFactory.getInstance().getPairingMultiplier(basePairing);
        for (int i = 0; i < v1.getSize(); i++) {
            combiner.addPairing(v1.getAt(i), v2.getAt(i));
        }
        return combiner.awaitResult();
    }

    public boolean isProductPairingSupported() {
        return false;
    }

    public Element pairing(Element[] in1, Element[] in2) {
        PairingAccumulator combiner = PairingAccumulatorFactory.getInstance().getPairingMultiplier(basePairing);
        for (int i = 0; i < in1.length; i++) {
            combiner.addPairing(in1[i], in2[i]);
        }
        return combiner.awaitResult();
    }

    public PairingPreProcessing getPairingPreProcessingFromElement(final Element in1) {
        return new DefaultPairingPreProcessing(this, in1);
    }

    public int getPairingPreProcessingLengthInBytes() {
        throw new IllegalStateException("Not implemented yet!!!");
    }

    public PairingPreProcessing getPairingPreProcessingFromBytes(byte[] source) {
        throw new IllegalStateException("Not implemented yet!!!");
    }

    public PairingPreProcessing getPairingPreProcessingFromBytes(byte[] source, int offset) {
        throw new IllegalStateException("Not implemented yet!!!");
    }

    public int getFieldIndex(Field field) {
        throw new IllegalStateException("Not Implemented yet!");
    }

}
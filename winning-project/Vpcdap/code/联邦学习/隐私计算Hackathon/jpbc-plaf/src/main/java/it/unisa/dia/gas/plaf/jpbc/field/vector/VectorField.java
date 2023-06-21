package it.unisa.dia.gas.plaf.jpbc.field.vector;

import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.plaf.jpbc.field.base.AbstractFieldOver;

import java.math.BigInteger;
import java.security.SecureRandom;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class VectorField<F extends Field> extends AbstractFieldOver<F, VectorElement> {
    protected int n, lenInBytes;


    public VectorField(SecureRandom random, F targetField, int n) {
        super(random, targetField);

        this.n = n;
        this.lenInBytes = n * targetField.getLengthInBytes();
    }


    public VectorElement newElement() {
        return new VectorElement(this);
    }

    public BigInteger getOrder() {
        throw new IllegalStateException("Not implemented yet!!!");
    }

    public VectorElement getNqr() {
        throw new IllegalStateException("Not implemented yet!!!");
    }

    public int getLengthInBytes() {
        return lenInBytes;
    }

    public int getN() {
        return n;
    }

}

package it.unisa.dia.gas.plaf.jpbc.pairing.immutable;

import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.ElementPowPreProcessing;
import it.unisa.dia.gas.jpbc.Field;

import java.math.BigInteger;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 * @since 2.0.0
 */
public class ImmutableElementPowPreProcessing implements ElementPowPreProcessing {

    private ElementPowPreProcessing elementPowPreProcessing;
    private Field immutableField;

    public ImmutableElementPowPreProcessing(ImmutableField immutableField, ElementPowPreProcessing elementPowPreProcessing){
        this.immutableField = immutableField;
        this.elementPowPreProcessing = elementPowPreProcessing;
    }

    public Field getField() {
        return immutableField;
    }

    public Element pow(BigInteger n) {
        return elementPowPreProcessing.pow(n).getImmutable();
    }

    public Element powZn(Element n) {
        return elementPowPreProcessing.powZn(n).getImmutable();
    }

    public byte[] toBytes() {
        return elementPowPreProcessing.toBytes();
    }
}

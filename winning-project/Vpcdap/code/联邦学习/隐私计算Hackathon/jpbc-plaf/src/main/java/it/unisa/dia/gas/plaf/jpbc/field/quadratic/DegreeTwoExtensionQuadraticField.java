package it.unisa.dia.gas.plaf.jpbc.field.quadratic;

import it.unisa.dia.gas.jpbc.Field;

import java.security.SecureRandom;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class DegreeTwoExtensionQuadraticField<F extends Field> extends QuadraticField<F, DegreeTwoExtensionQuadraticElement> {

    public DegreeTwoExtensionQuadraticField(SecureRandom random, F targetField) {
        super(random, targetField);
    }


    public DegreeTwoExtensionQuadraticElement newElement() {
        return new DegreeTwoExtensionQuadraticElement(this);
    }

}

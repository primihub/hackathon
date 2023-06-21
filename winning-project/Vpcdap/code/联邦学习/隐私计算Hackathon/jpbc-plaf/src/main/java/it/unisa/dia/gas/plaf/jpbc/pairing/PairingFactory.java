package it.unisa.dia.gas.plaf.jpbc.pairing;

import it.unisa.dia.gas.jpbc.Pairing;
import it.unisa.dia.gas.jpbc.PairingParameters;
import it.unisa.dia.gas.plaf.jpbc.pairing.a.TypeAPairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.a1.TypeA1Pairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.d.TypeDPairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.e.TypeEPairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.f.TypeFPairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.g.TypeGPairing;
import it.unisa.dia.gas.plaf.jpbc.pairing.immutable.ImmutableParing;
import it.unisa.dia.gas.plaf.jpbc.pairing.parameters.PropertiesParameters;

import java.lang.reflect.Method;
import java.security.SecureRandom;
import java.util.HashMap;
import java.util.Map;

/**
 * @author Angelo De Caro (jpbclib@gmail.com)
 */
public class PairingFactory {
    private static final PairingFactory INSTANCE = new PairingFactory();


    public static PairingFactory getInstance() {
        return INSTANCE;
    }

    public static Pairing getPairing(PairingParameters parameters) {
        return getInstance().initPairing(parameters);
    }

    public static Pairing getPairing(String parametersPath) {
        return getInstance().initPairing(parametersPath);
    }

    public static Pairing getPairing(PairingParameters parameters, SecureRandom random) {
        return getInstance().initPairing(parameters, random);
    }

    public static Pairing getPairing(String parametersPath, SecureRandom random) {
        return getInstance().initPairing(parametersPath, random);
    }

    public static PairingParameters getPairingParameters(String parametersPath) {
        return getInstance().loadParameters(parametersPath);
    }


    private boolean usePBCWhenPossible = false;
    private boolean reuseInstance = true;
    private boolean pbcAvailable = false;
    private boolean immutable = false;

    private Map<PairingParameters, Pairing> instances;
    private Map<String, PairingCreator> creators;
    private SecureRandomCreator secureRandomCreator;


    private PairingFactory() {
        this.instances = new HashMap<PairingParameters, Pairing>();
        this.creators = new HashMap<String, PairingCreator>();
        this.secureRandomCreator = new DefaultSecureRandomCreator();

        PairingCreator defaultCreator = new EllipticCurvesPairingCreator();
        creators.put("a", defaultCreator);
        creators.put("a1", defaultCreator);
        creators.put("d", defaultCreator);
        creators.put("e", defaultCreator);
        creators.put("f", defaultCreator);
        creators.put("g", defaultCreator);

        creators.put("ctl13", new CTL13MultilinearPairingCreator());
    }


    public Pairing initPairing(String parametersPath) {
        return initPairing(loadParameters(parametersPath), secureRandomCreator.newSecureRandom());
    }

    public Pairing initPairing(PairingParameters parameters) {
        return initPairing(parameters, secureRandomCreator.newSecureRandom());
    }

    public Pairing initPairing(String parametersPath, SecureRandom random) {
        return initPairing(loadParameters(parametersPath), random);
    }

    public Pairing initPairing(PairingParameters parameters, SecureRandom random) {
        if (parameters == null)
            throw new IllegalArgumentException("parameters cannot be null.");

        if (random == null)
            random = secureRandomCreator.newSecureRandom();

        Pairing pairing = null;
        if (reuseInstance) {
            pairing = instances.get(parameters);
            if (pairing != null)
                return pairing;
        }

        String type = parameters.getString("type");
        PairingCreator creator = creators.get(type);
        if (creator == null)
            throw new IllegalArgumentException("Type not supported. Type = " + type);

        pairing = creator.create(type, random, parameters);
        if (pairing == null)
            throw new IllegalArgumentException("Cannot create pairing instance. Type = " + type);

        if (immutable)
            pairing = new ImmutableParing(pairing);

        if (reuseInstance)
            instances.put(parameters, pairing);

        return pairing;
    }


    public PairingParameters loadParameters(String path) {
        PropertiesParameters curveParams = new PropertiesParameters();
        curveParams.load(path);

        return curveParams;
    }


    public boolean isPBCAvailable() {
        return pbcAvailable;
    }

    public boolean isUsePBCWhenPossible() {
        return usePBCWhenPossible;
    }

    public void setUsePBCWhenPossible(boolean usePBCWhenPossible) {
        this.usePBCWhenPossible = usePBCWhenPossible;
    }

    public boolean isReuseInstance() {
        return reuseInstance;
    }

    public void setReuseInstance(boolean reuseInstance) {
        this.reuseInstance = reuseInstance;
    }

    public boolean isImmutable() {
        return immutable;
    }

    public void setImmutable(boolean immutable) {
        this.immutable = immutable;
    }


    public void addPairingCreator(String type, PairingCreator creator) {
        creators.put(type, creator);
    }


    public static interface PairingCreator {

        Pairing create(String type, SecureRandom random, PairingParameters pairingParameters);

    }

    public static class CTL13MultilinearPairingCreator implements PairingCreator {

        private Method getPairingMethod;
        private Throwable throwable;

        public CTL13MultilinearPairingCreator() {
            try {
                Class pbcPairingFactoryClass = Class.forName("it.unisa.dia.gas.plaf.jpbc.mm.clt13.pairing.CTL13PairingFactory");
                getPairingMethod = pbcPairingFactoryClass.getMethod("getPairing", SecureRandom.class, PairingParameters.class);
            } catch (Exception e) {
                throwable = e;
            }

        }

        public Pairing create(String type, SecureRandom random, PairingParameters parameters) {
            try {
                return (Pairing) getPairingMethod.invoke(null, random, parameters);
            } catch (Exception e) {
                // Ignore
                e.printStackTrace();
            }
            return null;
        }

        public Throwable getThrowable() {
            return throwable;
        }
    }

    public class EllipticCurvesPairingCreator implements PairingCreator {

        private Method getPairingMethod;
        private Throwable pbcPairingFailure;

        public EllipticCurvesPairingCreator() {
            // Try to load jpbc-pbc factory
            try {
                Class pbcPairingFactoryClass = Class.forName("it.unisa.dia.gas.plaf.jpbc.pbc.PBCPairingFactory");
                Method isPBCAvailable = pbcPairingFactoryClass.getMethod("isPBCAvailable", null);

                pbcAvailable = ((Boolean) isPBCAvailable.invoke(null));
                if (pbcAvailable)
                    getPairingMethod = pbcPairingFactoryClass.getMethod("getPairing", PairingParameters.class);
            } catch (Exception e) {
                pbcAvailable = false;
                pbcPairingFailure = e;
            }

        }

        public Pairing create(String type, SecureRandom random, PairingParameters parameters) {
            Pairing pairing = null;

            // Handle bilinear maps parameters
            if (usePBCWhenPossible && pbcAvailable)
                pairing = getPBCPairing(parameters);

            if (pairing == null) {
                if ("a".equalsIgnoreCase(type))
                    pairing = new TypeAPairing(random, parameters);
                else if ("a1".equalsIgnoreCase(type))
                    pairing = new TypeA1Pairing(random, parameters);
                else if ("d".equalsIgnoreCase(type))
                    pairing = new TypeDPairing(random, parameters);
                else if ("e".equalsIgnoreCase(type))
                    pairing = new TypeEPairing(random, parameters);
                else if ("f".equalsIgnoreCase(type))
                    return new TypeFPairing(random, parameters);
                else if ("g".equalsIgnoreCase(type))
                    return new TypeGPairing(random, parameters);
                else
                    throw new IllegalArgumentException("Type not supported. Type = " + type);
            }

            return pairing;
        }

        public Pairing getPBCPairing(PairingParameters parameters) {
            try {
                return (Pairing) getPairingMethod.invoke(null, parameters);
            } catch (Exception e) {
                // Ignore
                e.printStackTrace();
            }
            return null;
        }

        public Throwable getPbcPairingFailure() {
            return pbcPairingFailure;
        }
    }


    public static interface SecureRandomCreator {

        SecureRandom newSecureRandom();

    }

    public static class DefaultSecureRandomCreator implements SecureRandomCreator {

        public SecureRandom newSecureRandom() {
            return new SecureRandom();
        }
    }

}

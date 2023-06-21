import java.math.BigInteger;
import java.util.Random;

/**
 * @BelongsProject: Secure_Aggregation
 * @BelongsPackage: com.duwei.crypto
 * @Author: duwei
 * @Date: 2022/4/28 16:37
 * @Description: Shamir秘密共享方法
 */
public class SecretShare {
    private static BigInteger p;
    private static Random random;

    /**
     * 秘密分享算法
     *
     * @param secret 秘密
     * @param m      系统总用户数
     * @param t      秘密分享的阈值
     * @return m个用户的秘密数组
     */
    public static BigInteger[] share(BigInteger secret, int m, int t) {
        //储存t - 1次多项系系数
        BigInteger[] coefficients = new BigInteger[t];
        coefficients[0] = secret;
        for (int i = 1; i < t; i++) {
            coefficients[i] = generateRandomBigInteger();
        }

        BigInteger[] userShares = new BigInteger[m];
        //进行秘密分享
        for (int i = 0; i < m; i++) {
            userShares[i] = computeShare(coefficients, (i + 1));
        }

        return userShares;
    }

    /**
     * 为指定用户计算秘密份额
     *
     * @param coefficients 系数向量
     * @param userIndex    用户索引，即对于用户的x值
     *                     这里计算f(x)时x取值为 (下标 + 1)
     *                     如果直接从下标开始不加1会 直接暴露出 f(0)即秘密
     * @return
     */
    public static BigInteger computeShare(BigInteger[] coefficients, int userIndex) {
        BigInteger index = new BigInteger(String.valueOf(userIndex));
        int len = coefficients.length;
        BigInteger temp = BigInteger.ONE;
        BigInteger result = BigInteger.ZERO;
        for (int i = 0; i < len; i++) {
            BigInteger cur = coefficients[i].multiply(temp);
            temp = temp.multiply(index);
            result = result.add(cur).mod(p);
        }
        return result.mod(p);
    }

    /**
     * 生成小于p的随机数
     *
     * @return
     */
    public static BigInteger generateRandomBigInteger() {
        BigInteger result;
        do {
            result = new BigInteger(p.bitLength(), random);
        } while ((result.compareTo(p) >= 0) && (result.compareTo(BigInteger.ZERO) != 0));
        return result;
    }

    /**
     * 初始化方法，指定模数的bit长度
     * @param bitLen
     */
    public static void init(int bitLen) {
        random = new Random();
        p = BigInteger.probablePrime(bitLen,random);
    }

    /**
     * 秘密重建算法
     *
     * @param shares 用户输入的秘密
     * @param t      可以恢复秘密的阈值
     * @return
     */
    public static BigInteger reconstruction(BigInteger[] shares, int t) throws Exception {
        int n = shares.length;
        if (t > n) {
            throw new Exception("你当前收集的秘密份额不足以恢复秘密");
        }
        BigInteger result = new BigInteger("0");
        for (int i = 0; i < t; i++) {
            result = result.add(interpolation(shares, i + 1, t));
        }

        return result.mod(p);
    }

    /**
     * 求第i号用户(xK = i + 1)的了拉格朗日插值
     *
     * @param values
     * @param t
     * @return
     */
    public static BigInteger interpolation(BigInteger[] values, int xK, int t) {
        BigInteger result;
        //常量0，计算f(0)
        BigInteger zero = BigInteger.ZERO;
        BigInteger x_k = new BigInteger(String.valueOf(xK));
        //拉格朗日多项式
        BigInteger up = BigInteger.ONE;
        BigInteger down = BigInteger.ONE;
        //i代表第i个用户的份额
        for (int i = 0; i < t; i++) {
            BigInteger x_i = new BigInteger(String.valueOf((i + 1)));
            if (x_i.equals(x_k)) {
                continue;
            }
            up = up.multiply(zero.subtract(x_i));
            down = down.multiply(x_k.subtract(x_i));
        }
        result = up.multiply(down.modInverse(p));
        result = result.multiply(values[xK - 1]);
        return result;
    }

    public static void main(String[] args) throws Exception {
        init(1024);
        int times = 1;
        System.out.println("测试开始.....");
        for (int i = 0;i < times;i++){
            //生成小于p的随机秘密
            BigInteger secret = generateRandomBigInteger();
            int m = (int) (Math.random() * 100 ) + 5;
            int t = (int) (Math.random() * 50 ) + 1;
            //阈值必须小于总用户数
            while (t > m){
                t = (int) (Math.random() * 50 ) + 1;
            }

            BigInteger[] shares = share(secret, m, t);
            BigInteger reconstruction = reconstruction(shares, t);
            if (reconstruction.compareTo(secret) != 0){
                System.out.println("秘密值恢复错误");
            }
        }
        System.out.println("测试结束.....");
    }
}




import it.unisa.dia.gas.jpbc.Element;
import it.unisa.dia.gas.jpbc.Field;
import it.unisa.dia.gas.jpbc.Pairing;
import it.unisa.dia.gas.jpbc.PairingParameters;
import it.unisa.dia.gas.plaf.jpbc.pairing.PairingFactory;
import it.unisa.dia.gas.plaf.jpbc.pairing.a.TypeACurveGenerator;


import java.math.BigInteger;
import java.security.MessageDigest;
import java.util.Random;


public class main {
    public static Pairing pairing;
    public static Field<Element> G1,GT,Zq;
    public static Element g;//G1的生成元
    public static int N=12;//一共N个用户
    public static int t=N/2;//秘密分享的阈值
    public static int id_length=8;//用户id长度


    public static byte[] toByteArr(int i) {
        //int转byte数组
        // 因为一个 int 类型是 4个 byte 所以这里要开辟 4位 空间
        byte[] b = new byte[4];
        // 16 进制的 0xFF 也就是 2进制中的 1111 1111 这一步是获得前8位的二进制数据
        b[0] = (byte) ((i >>> 24) & 0xFF);
        // 同理 这一步是获取 第9位到第16位的二进制数据
        b[1] = (byte) ((i >>> 16) & 0xFF);
        // ...
        b[2] = (byte) ((i >>> 8) & 0xFF);
        b[3] = (byte) (i & 0xFF);
        // 因为 int 是 32 个 bit 所以到这里就获取到 字节数组了
        return b;
    }


    public static byte[] arrayconnect3(byte[] b1,byte[] b2,byte[] b3){
        //连接3个字节数组
        byte[] all = new byte[b1.length+b2.length+b3.length];
        System.arraycopy(b1,0,all,0,b1.length);
        System.arraycopy(b2,0,all,b1.length,b2.length);
        System.arraycopy(b3,0,all,b1.length+b2.length,b3.length);
        return all;
    }

    //论文中的H1函数
    public static Element H1( Element zq_element) {
        // H1 : Zq -> G1
        byte[] zq_bytes = zq_element.getImmutable().toCanonicalRepresentation();
        byte[] g1_bytes = zq_bytes;
        try {
            MessageDigest hasher = MessageDigest.getInstance("SHA-512");
            g1_bytes = hasher.digest(zq_bytes);   //先把Zq元素hash成512bits
        } catch (Exception e) {
            e.printStackTrace();
        }
        //再把hash后的bits映射到G1
        Element hash_result = pairing.getG1().newElementFromHash(g1_bytes, 0, g1_bytes.length).getImmutable();
        return hash_result;
    }

    //论文中的H2函数
    public static Element H2( Element g1_element) {
        // H2 : G1 -> Zq
        byte[] g1_bytes = g1_element.getImmutable().toCanonicalRepresentation();
        byte[] zq_bytes = g1_bytes;
        try {
            MessageDigest hasher = MessageDigest.getInstance("SHA-512");
            zq_bytes = hasher.digest(g1_bytes);   //先把G1元素hash成512bits
        } catch (Exception e) {
            e.printStackTrace();
        }
        //再把hash后的bits映射到Zp
        Element hash_result = pairing.getZr().newElementFromHash(zq_bytes, 0, zq_bytes.length).getImmutable();
        return hash_result;
    }

    //论文中的H3函数
    public static Element H3(BigInteger Ui,Element Xi,Element Ii) {
        // H3 : Zq*G1*G1 -> Zq
        byte[] xi_bytes = Xi.getImmutable().toCanonicalRepresentation();
        byte[] ii_bytes = Ii.getImmutable().toCanonicalRepresentation();
        /*
        BigInteger test=new BigInteger("12138",10);
        System.out.println(Arrays.toString(toByteArr(12138)));
        System.out.println(Arrays.toString(test.toByteArray()));
        System.out.println(new BigInteger(toByteArr(12138)));
         /**/
        byte[] ui_bytes = Ui.toByteArray();
        byte[] all = arrayconnect3(ui_bytes,xi_bytes,ii_bytes);
        byte[] All=all;
        try {
            MessageDigest hasher = MessageDigest.getInstance("SHA-512");
            All = hasher.digest(all);   //先把元素hash成512bits
        } catch (Exception e) {
            e.printStackTrace();
        }
        //System.out.println(Arrays.toString(Ui_bytes)+Arrays.toString(Xi_bytes)+Arrays.toString(Ii_bytes));
        //System.out.println(Arrays.toString(all));
        //再把hash后的bits映射到Zp
        Element hash_result = pairing.getZr().newElementFromHash(All, 0, All.length).getImmutable();
        return hash_result;
    }

    //论文中的H4函数
    public static Element H4( Element zq_element) {
        // H4 : Zq -> Zq
        byte[] g1_bytes = zq_element.getImmutable().toCanonicalRepresentation();
        byte[] zq_bytes = g1_bytes;
        try {
            MessageDigest hasher = MessageDigest.getInstance("SHA-512");
            zq_bytes = hasher.digest(g1_bytes);   //先把Zq元素hash成512bits
        } catch (Exception e) {
            e.printStackTrace();
        }
        //再把hash后的bits映射到Zp
        Element hash_result = pairing.getZr().newElementFromHash(zq_bytes, 0, zq_bytes.length).getImmutable();
        return hash_result;
    }

    //论文中的H5函数
    public static Element H5(BigInteger zq_element){
        return g.pow(zq_element);
    }

    //论文中的KA.agree，DH密钥协商算法
    public static Element agree(Element di,Element dj){
        //模拟dh交换协议
        Element res=dj.powZn(di).getImmutable();
        return res;
    }

    //论文中的AE.enc算法，是一个对称加密算法，这里采用直接和密钥异或
    public static BigInteger enc(Element key,BigInteger Ui,BigInteger Uj,BigInteger xij){
        //对称加密：异或
        //因为是异或加密所以不要直接BigInteger类型的，建议转换为字符串或者string类型，然后对比长度来进行异或操作
        //直接使用BigInteger自带的xor会无法对点坐标进行运算
        String mess=Ui.toString(10)+Uj.toString(10)+xij.toString(10);
        BigInteger M=new BigInteger(mess,10);
        String k=new BigInteger(key.toBytes()).toString();
        String Key="";
        int lm=mess.length();
        int time=lm/k.length();
        //time=0;
        for (int i = 0; i < time+1; i++) {
            Key=Key+k;
        }
        BigInteger K=new BigInteger(Key,10);
        //System.out.println(K.toString(10).length()==lm);
        return M.xor(K);
    }

    //论文中的AE.enc算法，是一个对称加密算法，这里采用直接和密钥异或
    public static BigInteger[] dec(Element key,BigInteger Pij){
        //对称解密：异或
        //返回的数组内容分别是{ui,uj,xij}
        String mess=Pij.toString(10);
        BigInteger M=new BigInteger(mess,10);
        String k=new BigInteger(key.toBytes()).toString();
        String Key="";
        int lm=mess.length();
        int time=lm/k.length();
        time=0;
        for (int i = 0; i < time+1; i++) {
            Key=Key+k;
        }
        BigInteger K=new BigInteger(Key,10);
        //System.out.println("dec:"+K);
        String mess1=M.xor(K).toString(10);
        BigInteger[] res=new BigInteger[3];
        res[0]=new BigInteger(mess1.substring(0,id_length));
        res[1]=new BigInteger(mess1.substring(id_length,2*id_length));
        res[2]=new BigInteger(mess1.substring(2*id_length));
        return res;
    }

    //论文中的PRG函数，以sij作为随机数种子生成随机数。这里采用直接转化大数字
    public static BigInteger PRG(Element sij){
        //PRG函数
        //new BigInteger(sij.toBytes())
        //return new BigInteger(512,new Random());
        return new BigInteger(sij.toBytes());
    }

    public static void main(String[] args) {
        //自定义曲线参数
        int rBits = 160;
        int qBits = 512;  //对应论文中的q
        TypeACurveGenerator pg = new TypeACurveGenerator(rBits, qBits);
        PairingParameters pp = pg.generate();
        pairing = PairingFactory.getPairing(pp);

        //3个相关的群
        G1=pairing.getG1();  //G1群
        GT=pairing.getGT();  //GT群
        Zq=pairing.getZr();  //Zq群

        //g是G1的生成元
        g=G1.newRandomElement().getImmutable();


        //开始计时
        long start = System.currentTimeMillis();


        //KGC初始化阶段
        Element K1=H1(Zq.newRandomElement()).getImmutable();  //论文中KGC初始化生成的K1
        Element K2=H1(Zq.newRandomElement()).getImmutable();  //论文中KGC初始化生成的K2
        Element s=Zq.newRandomElement().getImmutable();       //论文中KGC初始化生成的s
        Element P=g.powZn(s).getImmutable();


        //密钥生成阶段
        //用户操作：
        BigInteger u0=new BigInteger("9999999",10);  //第一个用户是10000000，其他用户依次+1
        BigInteger one=new BigInteger("1",10);
        //所有用户的数据
        BigInteger[] Ui=new BigInteger[N];  //Ui是论文中的ui，指用户id
        Element[] xi=new Element[N];        //论文中的xi，用户生成的随机Zq群元素
        Element[] Xi=new Element[N];        //论文中的Xi，用户计算的G1群元素
        Ui[0]=u0.add(one);
        xi[0]=Zq.newRandomElement().getImmutable();
        Xi[0]=g.powZn(xi[0]).getImmutable();
        for (int i = 1; i < N; i++) {
            Ui[i]=Ui[i-1].add(one);
            xi[i]=Zq.newRandomElement().getImmutable();
            Xi[i]=g.powZn(xi[i]).getImmutable();
        }
        /*
        Element xi=Zq.newRandomElement().getImmutable();
        Element Xi=g.powZn(xi).getImmutable();
         */
        //KGC操作：（接收到Xi）
        Element[] ii=new Element[N];    //论文中的小写希腊字母phi，KGC选择的Zq群元素
        Element[] Ii=new Element[N];    //论文中的大写希腊字母phi，KGC计算的G1群元素
        Element[] hi=new Element[N];    //指论文中hi，是H3哈希函数的计算值，即：hi=H3(Ui||Xi||Ii)
        Element[] ki=new Element[N];    //论文中的ki，即ki=ii+s*hi+H2(Xi^s)
        for (int i = 0; i < N; i++) {
            ii[i]=Zq.newRandomElement().getImmutable();
            Ii[i]=g.powZn(ii[i]).getImmutable();
            hi[i]=H3(Ui[i],Xi[i],Ii[i]).getImmutable();
            ki[i]=ii[i].add(s.mulZn(hi[i])).add(H2(Xi[i].powZn(s))).getImmutable();
        }
        //Element k0=ii[0].add(s.mulZn(hi[0])).add(H2(Xi[0].powZn(s))).getImmutable();//单节点只涉及节点自己的k值
        /*
        Element ii=Zq.newRandomElement().getImmutable();
        Element Ii=g.powZn(ii).getImmutable();
        Element hi=H3(ui,Xi,Ii).getImmutable();
        Element ki=ii.add(s.mulZn(hi)).add(H2(Xi.powZn(s))).getImmutable();
         */
        //用户操作：（接收到ki和Ii）
        Element[] di=new Element[N];  //用户计算的di，是Zq群元素，即di=ki-H2(P^xi)
        for (int i = 0; i < N; i++) {
            di[i]=ki[i].sub(H2(P.powZn(xi[i]))).getImmutable();
        }
        //Element d0=k0.sub(H2(P.powZn(xi[0])));
        if(!g.powZn(di[0]).equals(Ii[0].mul(P.powZn(hi[0])))) {
            System.out.println("密钥生成阶段用户0操作失败");
            return;
        }else {
            System.out.println("密钥生成阶段用户0操作成功");
        }
        Element[] Di=new Element[N];  //Di是G1群元素，指论文中的Di，即Di=K1^di
        for (int i = 0; i < N; i++) {
            Di[i]=K1.powZn(di[i]).getImmutable();
        }
        //Element D0=K1.powZn(d0).getImmutable();
        /*
        Element di=ki.sub(H2(P.powZn(xi)));
        if(!g.powZn(di).equals(Ii.mul(P.powZn(hi)))) {
            System.out.println("密钥生成阶段用户操作失败");
            return;
        }else {
            System.out.println("密钥生成阶段用户操作成功");
        }
        Element Di=K1.powZn(di).getImmutable();

         */

        //密钥共享阶段
        //用户操作
        SecretShare.init(512);//多项式秘密分享初始化mod p的p值
        BigInteger[] x0j = SecretShare.share(xi[0].toBigInteger(), N, t);  //多项式秘密分享，计算每个秘密份额xij,且j不等于i，其中ui=0
        BigInteger[] Pij=new BigInteger[N];//Pij,且j不等于i
        Element[] Kij=new Element[N];//Kij是协商密钥，且j不等于i
        Pij[0]=new BigInteger("0");
        for (int j = 1; j < N; j++) {//这里是以用户0的角度来计算，所以i不等于0
            Kij[j]=agree(di[0],Ii[j].mul(P.powZn(hi[j])));
            Pij[j]=enc(Kij[j],Ui[0],Ui[j],x0j[j]);
        }
        /*
        int T=1;
        //System.out.println(xij[3]);
        System.out.println((dec(Kij[T],Pij[T]))[2].equals(xij[T]));
        /**/


        //屏蔽输入阶段
        //用户操作
        System.out.println("双重掩蔽协议：进行加密");
        Element[][] sij=new Element[N][N];  //sij
        for (int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++){
                if (j==i){
                    continue;
                }
                sij[i][j]=agree(xi[i],Xi[j]);
            }
        }

        BigInteger[] mi=new BigInteger[N];
        for (int i = 0; i < N; i++) {
            mi[i]=new BigInteger(512,new Random());
        }
        BigInteger[] mi_=new BigInteger[N];  //mi_是用户计算的，论文中是顶上有^的mi
        for (int i = 0; i < N; i++){
            mi_[i]=new BigInteger(mi[i].toString(10),10);
            for(int j = i + 1; j < N; j++){
                mi_[i]=mi_[i].add(PRG(sij[i][j]));
            }
            for(int j = i - 1; j >= 0; j--){
                mi_[i]=mi_[i].subtract(PRG(sij[i][j]));
            }
        }
        /*
        BigInteger m0_=new BigInteger(mi[0].toString(10));
        for(int j = 1; j < N; j++){
            m0_=m0_.add(PRG(sij[j]));
        }
        for(int j = -1; j > 0; j--){
            m0_=m0_.subtract(PRG(sij[j]));
        }
        /* */


        //标签生成阶段
        //用户操作
        Element[] hi_=new Element[N];  //论文中的H4计算值，即hi_=H4(mi_)
        Element[] oi=new Element[N];  //论文中的sigma_i
        for (int i = 0; i < N; i++) {
            hi_[i]=H4(Zq.newOneElement().set(mi_[i]));
            oi[i]=Di[i].powZn(hi_[i]).mul(K2.powZn(xi[i])).mul(H5(mi[i]).getImmutable());
            //oi[i]=Di[i].powZn(hi_[i]).mul(K2.powZn(xi[i])).mul(g.pow(mi[i])).getImmutable();
        }
        /*
        Element h0_=H4(Zq.newOneElement().set(m0_).getImmutable());
        Element o0=Di[0].powZn(h0_).mul(K2.powZn(xi[0])).mul(H5(Zq.newOneElement().set(mi[0])));
        /**/


        //取消屏蔽阶段
        //用户操作
        //BigInteger[] temp;
        System.out.println("双重掩蔽协议：进行解密");
        for(int j = 1; j < N; j++){
            x0j[j] = dec(Kij[j],Pij[j])[2];
        }
        //CS操作
        BigInteger CS_x0=new BigInteger("0");
        try {
            CS_x0=SecretShare.reconstruction(x0j,t);
        }catch (Exception e){
            e.printStackTrace();
        }
        /*
        System.out.println(xi[0]);
        System.out.println(CS_x0);
        /**/


        //证明生成阶段
        //CS操作
        BigInteger m=new BigInteger("0");
        Element o=G1.newOneElement().getImmutable();  //论文中的sigma
        for (int i = 0; i < N; i++) {
            m=m.add(mi[i]);
            o=o.mul(oi[i]);
        }


        //验证阶段
        Element left;  //等式左边
        Element right;  //等式右边
        Element temp1=G1.newOneElement().getImmutable();
        Element temp2=G1.newOneElement().getImmutable();
        for (int i = 0; i < N; i++) {
            temp1=temp1.mul((Ii[i].mul(P.powZn(hi[i]))).powZn(hi_[i]));
            temp2=temp2.mul(Xi[i]);
        }
        right=pairing.pairing(K1,temp1).mul(pairing.pairing(K2,temp2)).mul(pairing.pairing(H5(m),g));
        //right=pairing.pairing(K1,temp1).mul(pairing.pairing(K2,temp2)).mul(pairing.pairing(g.pow(m),g));
        left=pairing.pairing(o,g);
        System.out.println("CS验证："+right.equals(left));

        //start = System.currentTimeMillis();
        //left=pairing.pairing(o,g);
        //结束计时
        long end = System.currentTimeMillis();
        long use_time=end-start;

        /*
        //
        Element T=GT.newOneElement().getImmutable();
        Element T0=G1.newOneElement().getImmutable();
        Element T1=G1.newOneElement().getImmutable();
        Element T2=G1.newOneElement().getImmutable();
        Element T0_=G1.newOneElement().getImmutable();
        Element T1_=G1.newOneElement().getImmutable();
        Element T2_=G1.newOneElement().getImmutable();

        Element TA = G1.newOneElement().getImmutable();
        Element TB = G1.newOneElement().getImmutable();

        for (int i = 0; i < N; i++) {
            //第二步正确
            //T=T.mul(pairing.pairing(Di[i].powZn(hi_[i]).mul(K2.powZn(xi[i])).mul(H5(Zq.newOneElement().set(mi[i]))),g));
            //第三步正确
            //T=T.mul(pairing.pairing(Di[i].powZn(hi_[i]),g)).mul(pairing.pairing(K2.powZn(xi[i]),g)).mul(pairing.pairing(H5(Zq.newOneElement().set(mi[i])),g));
            //第四步正确
            T0=T0.mul(K1.powZn(((s.mul(hi[i])).add(ii[i])).mul(hi_[i])));
            T1=T1.mul(K2.powZn(xi[i]));
            T2=T2.mul(H5(mi[i]));

            //第五步错误
            T0_=T0_.mul(g.powZn(((s.mul(hi[i])).add(ii[i])).mul(hi_[i])));
            T1_=T1_.mul(g.powZn(xi[i]));

        }
        //第四步正确
        //T=pairing.pairing(T0,g).mul(pairing.pairing(T1,g)).mul(pairing.pairing(T2,g));
        //第五步错误
        T2_=T2_.mul(H5(m));
        T=pairing.pairing(K1,T0_).mul(pairing.pairing(K2,T1_)).mul(pairing.pairing(T2_,g));

        //第五步验证

        System.out.println("第4步："+pairing.pairing(T0,g));
        System.out.println("第5步："+pairing.pairing(K1,T0_));
        System.out.println("第4步："+pairing.pairing(T1,g));
        System.out.println("第5步："+pairing.pairing(K2,T1_));
        System.out.println("第4步："+pairing.pairing(T2,g));
        System.out.println("第5步："+pairing.pairing(T2_,g));
        System.out.println(T);

        /**/
        System.out.println("总人数："+N+"\n门限值："+t);
        System.out.println("耗时："+use_time+"毫秒");


    }
}

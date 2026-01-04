package cn.simbok.demo;

import cn.simbok.irisHelper.Function2;
import cn.simbok.irisHelper.Function3;
import cn.simbok.irisHelper.Function4;
import cn.simbok.irisHelper.IrisHelper;

import java.util.List;

public class Iris {

    public static void main(String[] args) {
        Iris irisDemo = new Iris();

        irisDemo.init("{}");

        System.out.println("main started.");
    }

    IrisHelper mIris;

    public int changeConfigure(String cfg) {
        return mIris.changeConfigure(cfg);
    }

    public int loadDevParams(String devParams) {
        return mIris.loadDevParams(devParams);
    }

    public int init(String config) {
        mIris = new IrisHelper();

        try {
            int a = mIris.init(config, null, new Function2<Integer, Integer, Integer>() {
                @Override
                public Integer invoke(Integer event, Integer param) {
                    switch (event) {
                        case 1: {
                            System.out.println("unplugin");
                            mIris.release();
                            break;
                        }
                        case 2: {
                            System.out.println("plugin");
                            break;
                        }
                    }
                    return 0;
                }
            });

            if (a != 0) {
                String s = mIris.err2str(a);
                System.out.println(s);
            }

            return a;
        } catch (Exception e) {
            e.printStackTrace();

            return -1;
        }
    }

    public int enroll(String uid, boolean cover, Function4<String, Integer, Integer, Integer, Integer> callback) {
        return mIris.enroll(uid, cover, callback);
    }

    public int identify(String uid, boolean continuous, Function4<String, Integer, Integer, Integer, Integer> callback) {
        return mIris.identify(uid, continuous, callback);
    }

    public int setPreview(Function3<byte[], Integer, Integer, Integer> callback) {
        return mIris.setPreview(callback);
    }

    public void stop() {
        mIris.stop();
    }

    public List<String> getUserList() {
        return mIris.getUserList();
    }

    public int delete(String uid) {
        return mIris.deleteUser(uid);
    }
}

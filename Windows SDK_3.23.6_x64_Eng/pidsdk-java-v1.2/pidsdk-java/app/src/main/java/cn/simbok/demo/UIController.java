package cn.simbok.demo;

import javafx.application.Platform;
import javafx.beans.InvalidationListener;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.image.*;
import javafx.scene.paint.Color;

import java.io.*;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

public class UIController {
    @FXML
    public Button initButton;
    @FXML
    public Button enrollButton;
    @FXML
    public Button identifyButton;
    @FXML
    public Button stopButton;

    @FXML
    public ImageView irisImage;

    @FXML
    public ListView<String> userList;

    @FXML
    public Button refreshButton;
    @FXML
    public TextField userId;

    @FXML
    public Button deleteButton;

    @FXML
    private Label tipLabel;

    Iris iris = new Iris();

    private String readResourceString(String f) {
        InputStream ips = App.class.getResourceAsStream(f);
        BufferedReader br = new BufferedReader(new InputStreamReader(ips, StandardCharsets.UTF_8));
        StringBuilder sb = new StringBuilder();
        String line;
        try {
            while ((line = br.readLine()) != null) {
                sb.append(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return sb.toString();
    }

    @FXML
    protected void onInitButtonClicked() {
        String initS = readResourceString("param_common.cfg");
        int r = iris.init(initS);
        if (r == 0) {
            String devS = readResourceString("param_dev.cfg");
            int cr = iris.loadDevParams(devS);
            if (cr != 0) {
                tipLabel.setText(String.format("load dev params failed:%d", cr));
            }

            int rr = iris.setPreview((f, w, h) -> {
                WritableImage writableImage = new WritableImage(w, h);
                writableImage.getPixelWriter().setPixels(0, 0, w, h, PixelFormat.getByteBgraInstance(), f, 0, w * 4);
                irisImage.setImage(writableImage);
                irisImage.setRotate(180);
//                System.out.println(String.format("preview callback: %d", f.length));
                return 0;
            });
            tipLabel.setText(String.format("init success: %d", 0));

            userList.setItems(mIrisUsers);
            onRefreshButtonClicked();

        } else {

            tipLabel.setText(String.format("init failed: %d", r));
        }
    }

    @FXML
    protected void onEnrollButtonClicked() {
        String uid = userId.getText();
        int r = iris.enroll(uid, true, (userId, result, which, finished) -> {
            if (finished == 1) {
                Platform.runLater(new Runnable() {
                    @Override
                    public void run() {
                        if (result == 0) {
                            onRefreshButtonClicked();
                            tipLabel.setText(String.format("enroll success: %s, ret:%d", userId, result));
                        } else {
                            tipLabel.setText("retry please");
                        }
                    }
                });
            }
            return 0;
        });

        if (r == 0) {
            tipLabel.setText("start iris enroll");
        } else {
            tipLabel.setText(String.format("start enroll failed: %d", r));
        }
    }

    @FXML
    public void onIdentifyButtonClicked() {
        int r = iris.identify(null, true, (uid, result, which, finished) -> {
            if (finished == 1) {
                Platform.runLater(new Runnable() {
                    @Override
                    public void run() {
                        if (result == 0) {
                            tipLabel.setText(String.format("identify: %s", uid));
                        } else {
                            tipLabel.setText("");
                        }
                    }
                });
            }
            return 0;
        });

        if (r == 0) {
            tipLabel.setText("start iris identify");
        } else {
            tipLabel.setText(String.format("start iris identify failed: %d", r));
        }
    }

    ObservableList<String> mIrisUsers = FXCollections.observableArrayList();

    @FXML
    public void onStopButtonClicked() {
        iris.stop();
    }

    @FXML
    public void onRefreshButtonClicked() {
        List<String> l = iris.getUserList();
        if (!mIrisUsers.isEmpty()) {
            mIrisUsers.clear();
        }

        mIrisUsers.addAll(l);
    }

    @FXML
    public void onDeleteButtonClicked() {
        String u = userList.getSelectionModel().getSelectedItem();
        iris.delete(u);

        onRefreshButtonClicked();
    }
}
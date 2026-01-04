package cn.simbok.demo;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;

public class App extends Application {
    public static App INSTANCE;

    @Override
    public void start(Stage stage) throws IOException {
        INSTANCE = this;

        String userDir = System.getProperty("user.dir");
        System.out.println("user.dir: " + userDir);

        String libDir = System.getProperty("java.library.path");
        System.out.println("java.library.path: " + libDir);

        FXMLLoader fxmlLoader = new FXMLLoader(App.class.getResource("hello-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 1080, 720);
        stage.setTitle("Hello!");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}
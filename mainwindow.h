#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QScopedPointer>
#include <QLabel>
#include <QDebug>

#include "globals.h"
#include "deepneuralnetwormanager.h"
#include "opencvmanager.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT
    QPushButton *startDetectionPushButton;
    QPushButton *stopDetectionPushButton;
    QPushButton *initModelPushButton;
    QPushButton *testModelOnImagePushButton;
    QPushButton *chooseModelFilePushButton;
    QPushButton *chooseConfigureFilePushButton;
    QPushButton *chooseFileWithClassesNamesPushButton;
    QPushButton *openVideoFilePushButton;
    QPushButton *openTestImagePushButton;
    QLineEdit *modelFilePathLineEdit;
    QLineEdit *configFilePathLineEdit;
    QLineEdit *videoFilePathLineEdit;
    QLineEdit *testImageFilePathLineEdit;
    QLineEdit *pathToFileWithClassesNamesLineEdit;
    QDoubleSpinBox *confidenceChoiceSpinBox;
    QSpinBox *webCamNumberSpinBox;

    QGroupBox *webCamGroupBox;
    QGroupBox *videoFileGroupBox;

    QScopedPointer<OpenCVManager> openCVManager;
    QSharedPointer<DeepNeuralNetworManager> deepNeuralNetworkManager;

public:
    MainWindow(QWidget *parent = 0);
    void createLayout();
    void createConnections();
    void createWidgets();
    void initNeuralNetwork();
    void initCaptureDevice();

    ~MainWindow();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H

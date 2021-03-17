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
#include <QLabel>
#include <QDebug>

#include "globals.h"
#include "deepneuralnetwormanager.h"
#include "opencvmanager.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT
    QPushButton *startDetectionPushButton;
    QPushButton *chooseModelFilePushButton;
    QPushButton *chooseConfigureFilePushButton;
    QPushButton *openVideoFilePushButton;
    QLineEdit *modelFilePathLineEdit;
    QLineEdit *configFilePathLineEdit;
    QLineEdit *videoFilePathLineEdit;
    QDoubleSpinBox *confidenceChoiceSpinBox;
    QSpinBox *webCamNumberSpinBox;

    QGroupBox *webCamGroupBox;
    QGroupBox *videoFileGroupBox;

    QPointer<OpenCVManager> openCVManager;
    QPointer<DeepNeuralNetworManager> deepNeuralNetworkManager;

public:
    MainWindow(QWidget *parent = 0);
    void createLayout();
    void createConnections();
    void createWidgets();
    void initNeuralNetwork();
    void initCaptureDevice();

    ~MainWindow();
};

#endif // MAINWINDOW_H

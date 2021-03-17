#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),deepNeuralNetworkManager(new DeepNeuralNetworManager()),openCVManager(new OpenCVManager())
{

    createWidgets();
    createLayout();
    createConnections();

}

MainWindow::~MainWindow()
{

}

void MainWindow::createLayout()
{
    QVBoxLayout *mainWindowLayout = new QVBoxLayout();
    QWidget *mainWindowWidget = new QWidget();

    QGroupBox *configFilePathGroupBox = new QGroupBox("Выбор конфигурационного файла");
    QHBoxLayout *configFileHBoxLayout = new QHBoxLayout();
    configFileHBoxLayout->addWidget(configFilePathLineEdit);
    configFileHBoxLayout->addWidget(chooseConfigureFilePushButton);
    configFilePathGroupBox->setLayout(configFileHBoxLayout);

    QGroupBox *modelFilePathGroupBox = new QGroupBox("Выбор параметров модели");
    QHBoxLayout *modelFileHBoxLayout = new QHBoxLayout();
    modelFileHBoxLayout->addWidget(modelFilePathLineEdit);
    modelFileHBoxLayout->addWidget(chooseModelFilePushButton);
    modelFilePathGroupBox->setLayout(modelFileHBoxLayout);

    QGroupBox *chooseThresholdGroupBox = new QGroupBox("Выбор порогового уровня обнаружения");
    QHBoxLayout *thresholdLayout = new QHBoxLayout();
    thresholdLayout->addWidget(confidenceChoiceSpinBox);
    chooseThresholdGroupBox->setLayout(thresholdLayout);


    QHBoxLayout *webCamNumberLayout = new QHBoxLayout();
    QLabel *webCamNumberLabel = new QLabel("Номер устройства");
    webCamNumberLayout->addWidget(webCamNumberLabel);
    webCamNumberLayout->addStretch();
    webCamNumberLayout->addWidget(webCamNumberSpinBox);
    webCamGroupBox->setLayout(webCamNumberLayout);

    QHBoxLayout *videoFileLayout = new QHBoxLayout();
    videoFileLayout->addWidget(videoFilePathLineEdit);
    videoFileLayout->addWidget(openVideoFilePushButton);
    videoFileGroupBox->setLayout(videoFileLayout);

    QWidget *webVideoWidget = new QWidget();
    QHBoxLayout *webVideoWidgetLayout = new QHBoxLayout();
    webVideoWidgetLayout->addWidget(webCamGroupBox);
    webVideoWidgetLayout->addWidget(videoFileGroupBox);
    webVideoWidget->setLayout(webVideoWidgetLayout);

    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonWidgetLayout = new QHBoxLayout();
    buttonWidgetLayout->addStretch();
    buttonWidgetLayout->addWidget(startDetectionPushButton);
    buttonWidgetLayout->addStretch();
    buttonWidget->setLayout(buttonWidgetLayout);

    mainWindowLayout->addWidget(configFilePathGroupBox);
    mainWindowLayout->addWidget(modelFilePathGroupBox);
    mainWindowLayout->addWidget(chooseThresholdGroupBox);
    mainWindowLayout->addWidget(webVideoWidget);
    mainWindowLayout->addWidget(buttonWidget);

    mainWindowWidget->setLayout(mainWindowLayout);
    setCentralWidget(mainWindowWidget);

}

void MainWindow::createConnections()
{
    connect(chooseConfigureFilePushButton,&QPushButton::clicked,[&]()
    {
        auto configFilePath = QFileDialog::getOpenFileName(this,"Файл конфигурации","./","Config files: (*.cfg)");
        configFilePathLineEdit->setText(configFilePath);
    });

    connect(chooseModelFilePushButton,&QPushButton::clicked,[&]()
    {
        auto modelFilePath = QFileDialog::getOpenFileName(this,"Файл модели","./","Weights files: (*.weights)");
        modelFilePathLineEdit->setText(modelFilePath);
    });

    connect(openVideoFilePushButton,&QPushButton::clicked,[&]()
    {
        auto videoFilePath = QFileDialog::getOpenFileName(this,"Видеофайл","./","Video files: (*.avi)");
        videoFilePathLineEdit->setText(videoFilePath);
    });

    connect(videoFileGroupBox,&QGroupBox::toggled,[&](bool state)
    {
        webCamGroupBox->setChecked(!state);
    });

    connect(webCamGroupBox,&QGroupBox::toggled,[&](bool state)
    {
        videoFileGroupBox->setChecked(!state);
    });

    connect(startDetectionPushButton,&QPushButton::clicked,[&]()
    {
        initNeuralNetwork();
        initCaptureDevice();
    });

}

void MainWindow::createWidgets()
{
    chooseConfigureFilePushButton = new QPushButton("Выбрать");
    chooseModelFilePushButton = new QPushButton("Выбрать");
    openVideoFilePushButton = new QPushButton("Открыть");
    startDetectionPushButton = new QPushButton("Начать обнаружение");
    modelFilePathLineEdit = new QLineEdit();
    modelFilePathLineEdit->setReadOnly(true);
    modelFilePathLineEdit->setPlaceholderText("Путь к файлу с параметрами модели");
    configFilePathLineEdit = new QLineEdit();
    configFilePathLineEdit->setPlaceholderText("Путь к конфигурационному файлу");
    configFilePathLineEdit->setReadOnly(true);
    confidenceChoiceSpinBox = new QDoubleSpinBox();
    confidenceChoiceSpinBox->setMinimum(0);
    confidenceChoiceSpinBox->setMinimum(1);
    webCamGroupBox = new QGroupBox("Web-камера");
    webCamGroupBox->setCheckable(true);
    webCamGroupBox->setChecked(false);
    webCamNumberSpinBox = new QSpinBox();
    webCamNumberSpinBox->setMinimum(0);
    webCamNumberSpinBox->setMaximum(5);
    videoFileGroupBox = new QGroupBox("Видеофайл");
    videoFileGroupBox->setCheckable(true);
    videoFileGroupBox->setChecked(true);
    videoFilePathLineEdit = new QLineEdit();
    videoFilePathLineEdit->setPlaceholderText("Путь к видеофайлу");
}

void MainWindow::initNeuralNetwork()
{
    if(!modelFilePathLineEdit->text().isEmpty() && !configFilePathLineEdit->text().isEmpty())
    {
        deepNeuralNetworkManager->loadNet(modelFilePathLineEdit->text(),configFilePathLineEdit->text());
    }
}

void MainWindow::initCaptureDevice()
{
    if(webCamGroupBox->isChecked())
    {

        auto ret = openCVManager->openVideoCaputureDevice(SYS::VideoInputEnum::WEB);
        if(ret)
        {
            qDebug() << "Webcam is opened";
        }
        else
        {
            qDebug() << "Unable to open device";
        }
        return;
    }
    if(videoFileGroupBox->isChecked())
    {
        if(!videoFilePathLineEdit->text().isEmpty())
        {
            auto ret = openCVManager->openVideoCaputureDevice(SYS::VideoInputEnum::FILE,
                                                              videoFilePathLineEdit->text());
            if(ret)
            {
                qDebug() << "Videofile is opened";
            }
            else
            {
                qDebug() << "Unable to open videofile";
            }
            return;
        }
    }
}



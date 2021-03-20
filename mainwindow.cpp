#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    deepNeuralNetworkManager.reset(new DeepNeuralNetworManager());
    openCVManager.reset(new OpenCVManager());
    openCVManager->setDeepNetworkManager(deepNeuralNetworkManager);
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

    QGroupBox *modelInitializationGroupBox = new QGroupBox("Инициализация нейронной сети");
    QGridLayout *modelInitializationGroupBoxLayout = new QGridLayout();

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

    QGroupBox *classesNamesGroupBox = new QGroupBox("Выбор файлов с именами классов модели");
    QHBoxLayout *classesNamesGroupBoxLayout = new QHBoxLayout();
    classesNamesGroupBoxLayout->addWidget(pathToFileWithClassesNamesLineEdit);
    classesNamesGroupBoxLayout->addWidget(chooseFileWithClassesNamesPushButton);
    classesNamesGroupBox->setLayout(classesNamesGroupBoxLayout);

    QGroupBox *chooseThresholdGroupBox = new QGroupBox("Выбор порогового уровня обнаружения");
    QHBoxLayout *thresholdLayout = new QHBoxLayout();
    thresholdLayout->addWidget(confidenceChoiceSpinBox);
    chooseThresholdGroupBox->setLayout(thresholdLayout);

    modelInitializationGroupBoxLayout->addWidget(configFilePathGroupBox,0,0,1,1);
    modelInitializationGroupBoxLayout->addWidget(modelFilePathGroupBox,1,0,1,1);
    modelInitializationGroupBoxLayout->addWidget(classesNamesGroupBox,2,0,1,1);
    modelInitializationGroupBoxLayout->addWidget(chooseThresholdGroupBox,3,0,1,1);
    modelInitializationGroupBoxLayout->addWidget(initModelPushButton,4,0,1,1,Qt::AlignCenter);
    modelInitializationGroupBox->setLayout(modelInitializationGroupBoxLayout);

    QGroupBox *webVideoGroupBox = new QGroupBox("Выбор устройства захвата кадров");
    QHBoxLayout *webVideoWidgetLayout = new QHBoxLayout();

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

    webVideoWidgetLayout->addWidget(webCamGroupBox);
    webVideoWidgetLayout->addWidget(videoFileGroupBox);
    webVideoGroupBox->setLayout(webVideoWidgetLayout);

    QGroupBox *testOnImageGroupBox = new QGroupBox("Тест на изображении");
    QGridLayout *testOnImageWidgetLayout = new QGridLayout();
    testOnImageWidgetLayout->addWidget(testImageFilePathLineEdit,0,0,1,1);
    testOnImageWidgetLayout->addWidget(openTestImagePushButton,0,1,1,1);
    testOnImageWidgetLayout->addWidget(testModelOnImagePushButton,1,0,1,2,Qt::AlignCenter);
    testOnImageGroupBox->setLayout(testOnImageWidgetLayout);

    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonWidgetLayout = new QHBoxLayout();
    buttonWidgetLayout->addStretch();
    buttonWidgetLayout->addWidget(startDetectionPushButton);
    buttonWidgetLayout->addWidget(stopDetectionPushButton);
    buttonWidgetLayout->addStretch();
    buttonWidget->setLayout(buttonWidgetLayout);

    mainWindowLayout->addWidget(modelInitializationGroupBox);
    mainWindowLayout->addWidget(webVideoGroupBox);
    mainWindowLayout->addWidget(testOnImageGroupBox);
    mainWindowLayout->addWidget(buttonWidget);

    mainWindowWidget->setLayout(mainWindowLayout);
    setCentralWidget(mainWindowWidget);

}

void MainWindow::createConnections()
{
    connect(chooseFileWithClassesNamesPushButton,&QPushButton::clicked,[&]()
    {
        auto filePath = QFileDialog::getOpenFileName(this,"Файл с именами классов","./","Text files: *.txt");
        pathToFileWithClassesNamesLineEdit->setText(filePath);
    });
    connect(testModelOnImagePushButton,&QPushButton::clicked,[&]()
    {
        if(deepNeuralNetworkManager->networkIsValid() && !testImageFilePathLineEdit->text().isEmpty())
        {
            deepNeuralNetworkManager->testNetOnImage(testImageFilePathLineEdit->text());
        }
    });

    connect(initModelPushButton,&QPushButton::clicked,[&]()
    {
        initNeuralNetwork();

    });

    connect(openTestImagePushButton,&QPushButton::clicked,[&](){
        auto imagePath = QFileDialog::getOpenFileName(this,"Файл изображения","./","Image files: (*.jpg *.png)");
        testImageFilePathLineEdit->setText(imagePath);
    });

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

        initCaptureDevice();
        openCVManager->createOpenCVWindow();
        openCVManager->processVideoFrames();
    });

    connect(stopDetectionPushButton,&QPushButton::clicked,[&]()
    {
        openCVManager->stopCapturingFrames();
    });

    connect(openCVManager.data(),&OpenCVManager::lockMainWindow,[&](bool enabled)
    {
        this->setEnabled(enabled);
    });
}

void MainWindow::createWidgets()
{
    chooseConfigureFilePushButton = new QPushButton("Выбрать");
    chooseModelFilePushButton = new QPushButton("Выбрать");
    openVideoFilePushButton = new QPushButton("Открыть");
    chooseFileWithClassesNamesPushButton = new QPushButton("Открыть");
    startDetectionPushButton = new QPushButton("Начать обнаружение");
    stopDetectionPushButton = new QPushButton("Прекратить обнаружение");
    initModelPushButton = new QPushButton("Инициализировать");
    testModelOnImagePushButton = new QPushButton("Провести тест");
    openTestImagePushButton = new QPushButton("Открыть");
    modelFilePathLineEdit = new QLineEdit();
    modelFilePathLineEdit->setReadOnly(true);
    modelFilePathLineEdit->setPlaceholderText("Путь к файлу с параметрами модели");
    configFilePathLineEdit = new QLineEdit();
    configFilePathLineEdit->setPlaceholderText("Путь к конфигурационному файлу");
    configFilePathLineEdit->setReadOnly(true);
    confidenceChoiceSpinBox = new QDoubleSpinBox();
    confidenceChoiceSpinBox->setValue(0.25);
    confidenceChoiceSpinBox->setSingleStep(0.05);
    confidenceChoiceSpinBox->setMinimum(0);
    confidenceChoiceSpinBox->setMaximum(1);
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
    testImageFilePathLineEdit = new QLineEdit();
    testImageFilePathLineEdit->setReadOnly(true);
    testImageFilePathLineEdit->setPlaceholderText("Путь к тестовому изображению");
    pathToFileWithClassesNamesLineEdit = new QLineEdit();
    pathToFileWithClassesNamesLineEdit->setReadOnly(true);
    pathToFileWithClassesNamesLineEdit->setPlaceholderText("Путь к файлу с именами классов объектов");
}

void MainWindow::initNeuralNetwork()
{
    if(!modelFilePathLineEdit->text().isEmpty() && !configFilePathLineEdit->text().isEmpty() && !pathToFileWithClassesNamesLineEdit->text().isEmpty())
    {
        deepNeuralNetworkManager->loadNet(modelFilePathLineEdit->text(),configFilePathLineEdit->text());
        deepNeuralNetworkManager->setThreshold(confidenceChoiceSpinBox->value());
        deepNeuralNetworkManager->readFileWithClassesNames(pathToFileWithClassesNamesLineEdit->text());
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




void MainWindow::closeEvent(QCloseEvent *event)
{
     openCVManager->releaseResources();
}

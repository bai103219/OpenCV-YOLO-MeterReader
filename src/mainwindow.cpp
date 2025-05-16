#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <pylon/PylonIncludes.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <stdio.h>


#include <stdlib.h>
#include "settingdialog.h"

using namespace Pylon;
using namespace GenApi;
using namespace std;
using namespace cv;

#define APP_NAME "CameraView"

QSize smallSize;
QSize bigSize;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->installEventFilter(this);

    connect(ui->actionPreview,SIGNAL(triggered()),this,SLOT(startPreview()));
    connect(ui->actionRefresh,SIGNAL(triggered()),this,SLOT(refresh()));
    connect(ui->actionSetting,SIGNAL(triggered()),this,SLOT(setting()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    connect(ui->actionCollectSingle,SIGNAL(triggered()),this,SLOT(singleGrab()));
    connect(ui->actionCollectMulti,SIGNAL(triggered()),this,SLOT(multiGrab()));
    connect(ui->actionSpaceAlgo,SIGNAL(triggered()),this,SLOT(spatial_LSI_Matlab()));

    QToolBar *mytoolbar = new QToolBar(this);
    mytoolbar->addAction(ui->actionCloseAlgo);
    addToolBar(Qt::RightToolBarArea,mytoolbar);
    connect(ui->actionCloseAlgo,SIGNAL(triggered()),this,SLOT(algoArea()));

    smallSize = QSize(750,this->height());
    bigSize = this->size();


    algoArea();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){
    ui->sizeValue->setText("");
    ui->scaleValue->setText("");
    ui->collectionValue->setText("");
    ui->fpsValue->setText("");
//    ui->destAlriValue->setText("");
    ui->actionPreview->setEnabled(false);


    refresh();

//    CBaslerUsbCamera
}

void MainWindow::refresh(){

    if(FullNameOfSelectedDevice.length() > 0){
        m_camera.Close();
        m_camera.DetachDevice();
        FullNameOfSelectedDevice = "";
    }

    DeviceInfoList_t dList;
    CTlFactory::GetInstance().EnumerateDevices(dList,true);
     if ( dList.size() == 0 )
     {
         QMessageBox::warning(this,APP_NAME,"请插入Basler相机",QMessageBox::Close,QMessageBox::Accepted);
         setNoCamera();
     }
     else
     {
         try{
             CDeviceInfo info;
             info.SetDeviceClass( CBaslerUsbInstantCamera::DeviceClass());
             FullNameOfSelectedDevice = QString(info.GetFriendlyName());
             m_camera.Attach(CTlFactory::GetInstance().CreateFirstDevice(info));
             m_camera.Open();
             startPreview();
             //         for(uint i=0;i<dList.size();i++){
             //             FullNameOfSelectedDevice = QString(dList[i].GetFriendlyName());
             //             try
             //             {
             //                 CDeviceInfo info;
             //                 info.SetFriendlyName(FullNameOfSelectedDevice.toStdString().c_str());
             //                 m_camera.Attach(CTlFactory::GetInstance().CreateFirstDevice(info));

             //                 m_camera.Open();
             //                 usbCameraParam.Attach(&m_camera.GetNodeMap());
             //                 startPreview();
         }
         catch(GenICam::GenericException &e)
         {
             QMessageBox::warning(this,APP_NAME,e.GetDescription(),QMessageBox::Close,QMessageBox::Accepted);
             setNoCamera();
         }
     }
}


void MainWindow::readJson(){

}



// void MainWindow::startPreview(){
//     setButtons(true);
//     if(m_camera.IsGrabbing()){
//         m_camera.StopGrabbing();
//     }

//     try
//     {
//         INodeMap& nodemap = m_camera.GetNodeMap();

//         CIntegerPtr Width (nodemap.GetNode("Width"));
//         CIntegerPtr Height (nodemap.GetNode("Height"));
//         CFloatPtr Rate( nodemap.GetNode("AcquisitionFrameRate") );
//         //Acquisition frame rate of the camera in frames per second.
//         CBooleanPtr AcquisitionFrameRateEnable( nodemap.GetNode("AcquisitionFrameRateEnable") );
//         AcquisitionFrameRateEnable->FromString("true");
//         CFloatPtr ExposureTime( nodemap.GetNode("ExposureTime"));


//         Width->FromString(saveSettings->width);
//         Height->FromString(saveSettings->height);
//         ExposureTime->FromString(saveSettings->exposureTime);
//         Rate->FromString(saveSettings->acquisitionFrameRate);
//         //Exposure time of the camera in microseconds.

//         if(!saveSettings->myattr.isEmpty()){

//             if(saveSettings->type == 0){
//             CIntegerPtr Attr( nodemap.GetNode(saveSettings->myattr.toUtf8().constData()) );
//             Attr->FromString(saveSettings->myvalue.toUtf8().constData());
//             }else if(saveSettings->type == 1){
//             CFloatPtr Attr( nodemap.GetNode(saveSettings->myattr.toUtf8().constData()) );
//             Attr->FromString(saveSettings->myvalue.toUtf8().constData());
//             }else if(saveSettings->type == 2){
//             CBooleanPtr Attr( nodemap.GetNode(saveSettings->myattr.toUtf8().constData()) );
//             Attr->FromString(saveSettings->myvalue.toUtf8().constData());
//             }else if(saveSettings->type == 3){
//             CStringPtr Attr( nodemap.GetNode(saveSettings->myattr.toUtf8().constData()) );
//             Attr->FromString(saveSettings->myvalue.toUtf8().constData());
//             }
//         }


//         // 获取图像的原始尺寸
//         int originalWidth = Width->GetValue();
//         int originalHeight = Height->GetValue();

//         // 计算裁剪区域：从图像中心裁剪出所需的区域
//         int cropWidth = QString::fromStdString(saveSettings->width.c_str()).toInt();  // 使用设置的宽度
//         int cropHeight = QString::fromStdString(saveSettings->height.c_str()).toInt(); // 使用设置的高度

//         int cropX = (originalWidth - cropWidth) / 2; // 计算从中心开始裁剪的 X 坐标
//         int cropY = (originalHeight - cropHeight) / 2; // 计算从中心开始裁剪的 Y 坐标



//         CImageFormatConverter fc;
//         fc.OutputPixelFormat = PixelType_BGR8packed;
//         CPylonImage image;

//         // This smart pointer will receive the grab result data.
//         CGrabResultPtr ptrGrabResult;

//         m_camera.StartGrabbing(GrabStrategy_LatestImageOnly);

//         cv::Mat openCvImage;
//         cv::Mat openCvGrayImage;
//         int64_t i = 0;

//         while(m_camera.IsGrabbing())
//         {
//             m_camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

//             if (ptrGrabResult->GrabSucceeded())
//             {
//                 fc.Convert(image, ptrGrabResult);
//                 openCvImage= cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) image.GetBuffer());
//                 cvtColor(openCvImage, openCvImage, cv::COLOR_BGR2RGB);

//                 cv::Rect cropRect(cropX, cropY, cropWidth, cropHeight);
//                 cv::Mat croppedImage = openCvImage(cropRect);
//                 QImage qtImage(croppedImage.data, croppedImage.cols, croppedImage.rows, croppedImage.step, QImage::Format_RGB888);
//                 // QImage qtImage(openCvImage.data,openCvImage.cols,openCvImage.rows,openCvImage.step,QImage::Format_RGB888);
//                 ui->srcDisplay->setPixmap(QPixmap::fromImage(qtImage));
//                 ui->srcDisplay->update();
//                 ui->collectionValue->setText(QString("%1").arg(ptrGrabResult->GetBlockID()));

//                 float wScale = roundf( ui->srcDisplay->width()*100.0/Width->GetValue() )/100.0;
//                 float hScale = roundf( ui->srcDisplay->height()*100.0/Height->GetValue()) / 100.0;

//                 ui->scaleValue->setText(QString("W:%1 H:%2").arg(wScale).arg(hScale));
//                 ui->sizeValue->setText(QString("W:%1 H:%2").arg(ui->srcDisplay->width()).arg(ui->srcDisplay->height()));
//                 ui->fpsValue->setText(QString("%1").arg( round( Rate->GetValue(true) ) ));

//                 /*
//                 if(saveSettings->algo == Algorithm_AIHE::GRAY){
//                 cvtColor(openCvImage, openCvGrayImage, COLOR_RGB2GRAY);
//                 QImage destImage(openCvGrayImage.data,openCvGrayImage.cols,openCvGrayImage.rows,openCvGrayImage.step,QImage::Format_Indexed8);
//                 ui->destDisplay->setPixmap(QPixmap::fromImage(destImage));
//                 ui->destDisplay->update();
//                 ui->destAlriValue->setText("灰度图");
//                 }else if(saveSettings->algo == Algorithm_AIHE::SPACE){

//                 }
//                 */

//                 if(!ui->destDisplay->isHidden()){
//                     cvtColor(openCvImage, openCvGrayImage, COLOR_RGB2GRAY);
//                     openCvGrayImage = spatial_LSI(openCvImage,5);
//                     QImage destImage(openCvGrayImage.data,openCvGrayImage.cols,openCvGrayImage.rows,openCvGrayImage.step,QImage::Format_RGB32);
//                     ui->destDisplay->setPixmap(QPixmap::fromImage(destImage.scaled(ui->destDisplay->width(),ui->destDisplay->height())));
//                     ui->destDisplay->update();
//                 }

//             }
//             else
//             {
//                 cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
//             }
//             qApp->processEvents();

//         }
//     }
//     catch (GenICam::GenericException &e){
//     QMessageBox::warning(this,"",e.GetDescription(),QMessageBox::Cancel,QMessageBox::Accepted);
//     }
// }

void MainWindow::startPreview(){
    setButtons(true);
    if(m_camera.IsGrabbing()){
        m_camera.StopGrabbing();
    }

    try
    {
        INodeMap& nodemap = m_camera.GetNodeMap();

        // 先获取和设置基本参数
        CFloatPtr Rate(nodemap.GetNode("AcquisitionFrameRate"));
        CBooleanPtr AcquisitionFrameRateEnable(nodemap.GetNode("AcquisitionFrameRateEnable"));
        AcquisitionFrameRateEnable->FromString("true");
        CFloatPtr ExposureTime(nodemap.GetNode("ExposureTime"));
        ExposureTime->FromString(saveSettings->exposureTime);
        Rate->FromString(saveSettings->acquisitionFrameRate);

        // 获取传感器的完整分辨率
        CIntegerPtr WidthMax(nodemap.GetNode("WidthMax"));
        CIntegerPtr HeightMax(nodemap.GetNode("HeightMax"));
        int64_t sensorWidth = WidthMax->GetValue();
        int64_t sensorHeight = HeightMax->GetValue();

        // 确认用户设置的宽高是否合法
        int64_t cropWidth = atoi(saveSettings->width.c_str());
        int64_t cropHeight = atoi(saveSettings->height.c_str());
        
        // 确保尺寸不超过传感器范围
        if (cropWidth > sensorWidth) cropWidth = sensorWidth;
        if (cropHeight > sensorHeight) cropHeight = sensorHeight;

        // 根据相机规格计算有效的偏移量
        CIntegerPtr OffsetX(nodemap.GetNode("OffsetX"));
        CIntegerPtr OffsetY(nodemap.GetNode("OffsetY"));
        
        // 获取偏移量的增量和最大值
        int64_t offsetXInc = OffsetX->GetInc();
        int64_t offsetYInc = OffsetY->GetInc();
        int64_t offsetXMax = OffsetX->GetMax();
        int64_t offsetYMax = OffsetY->GetMax();
        
        // 计算偏移量，确保在相机允许的范围内
        int64_t offsetX = ((sensorWidth - cropWidth) / 2) / offsetXInc * offsetXInc; // 向下取整到增量的倍数
        int64_t offsetY = ((sensorHeight - cropHeight) / 2) / offsetYInc * offsetYInc;
        
        // 确保偏移量不超过最大值
        if (offsetX > offsetXMax) offsetX = offsetXMax;
        if (offsetY > offsetYMax) offsetY = offsetYMax;

        // 按照Basler相机的推荐顺序设置参数 - 先宽高再偏移
        CIntegerPtr Width(nodemap.GetNode("Width"));
        CIntegerPtr Height(nodemap.GetNode("Height"));
        
        // 先设置宽高
        Width->SetValue(cropWidth);
        Height->SetValue(cropHeight);
        
        // 再设置偏移量
        OffsetX->SetValue(offsetX);
        OffsetY->SetValue(offsetY);
        
        // 输出实际设置的值，用于Debug
        qDebug() << "设置ROI: " << cropWidth << "x" << cropHeight << " @ " << offsetX << "," << offsetY;
        qDebug() << "最大允许值: Width=" << Width->GetMax() << " Height=" << Height->GetMax() 
                 << " OffsetX=" << OffsetX->GetMax() << " OffsetY=" << OffsetY->GetMax();

        // 设置用户自定义参数
        if(!saveSettings->myattr.isEmpty()){
            if(saveSettings->type == 0){
                CIntegerPtr Attr(nodemap.GetNode(saveSettings->myattr.toUtf8().constData()));
                Attr->FromString(saveSettings->myvalue.toUtf8().constData());
            }else if(saveSettings->type == 1){
                CFloatPtr Attr(nodemap.GetNode(saveSettings->myattr.toUtf8().constData()));
                Attr->FromString(saveSettings->myvalue.toUtf8().constData());
            }else if(saveSettings->type == 2){
                CBooleanPtr Attr(nodemap.GetNode(saveSettings->myattr.toUtf8().constData()));
                Attr->FromString(saveSettings->myvalue.toUtf8().constData());
            }else if(saveSettings->type == 3){
                CStringPtr Attr(nodemap.GetNode(saveSettings->myattr.toUtf8().constData()));
                Attr->FromString(saveSettings->myvalue.toUtf8().constData());
            }
        }

        // 图像处理设置
        CImageFormatConverter fc;
        fc.OutputPixelFormat = PixelType_BGR8packed;
        CPylonImage image;

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;

        m_camera.StartGrabbing(GrabStrategy_LatestImageOnly);

        cv::Mat openCvImage;
        cv::Mat openCvGrayImage;
        int64_t i = 0;

        while(m_camera.IsGrabbing())
        {
            m_camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

            if (ptrGrabResult->GrabSucceeded())
            {
                fc.Convert(image, ptrGrabResult);
                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) image.GetBuffer());
                cvtColor(openCvImage, openCvImage, cv::COLOR_BGR2RGB);

                // 已经是裁剪后的图像，直接显示
                QImage qtImage(openCvImage.data, openCvImage.cols, openCvImage.rows, openCvImage.step, QImage::Format_RGB888);
                ui->srcDisplay->setPixmap(QPixmap::fromImage(qtImage));
                ui->srcDisplay->update();
                ui->collectionValue->setText(QString("%1").arg(ptrGrabResult->GetBlockID()));

                float wScale = roundf(ui->srcDisplay->width()*100.0/Width->GetValue())/100.0;
                float hScale = roundf(ui->srcDisplay->height()*100.0/Height->GetValue())/100.0;

                ui->scaleValue->setText(QString("W:%1 H:%2").arg(wScale).arg(hScale));
                ui->sizeValue->setText(QString("W:%1 H:%2").arg(ui->srcDisplay->width()).arg(ui->srcDisplay->height()));
                ui->fpsValue->setText(QString("%1").arg(round(Rate->GetValue(true))));

                if(!ui->destDisplay->isHidden()){
                    cvtColor(openCvImage, openCvGrayImage, COLOR_RGB2GRAY);
                    openCvGrayImage = spatial_LSI(openCvImage, 5);
                    QImage destImage(openCvGrayImage.data, openCvGrayImage.cols, openCvGrayImage.rows, openCvGrayImage.step, QImage::Format_RGB32);
                    ui->destDisplay->setPixmap(QPixmap::fromImage(destImage.scaled(ui->destDisplay->width(), ui->destDisplay->height())));
                    ui->destDisplay->update();
                }
            }
            else
            {
                cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
            }
            qApp->processEvents();
        }
    }
    catch (GenICam::GenericException &e){
        QMessageBox::warning(this, "", e.GetDescription(), QMessageBox::Cancel, QMessageBox::Accepted);
        qDebug() << "错误: " << e.GetDescription();
    }
}
void MainWindow::setButtons(bool inPreview){
    ui->actionPreview->setEnabled(!inPreview);
    ui->actionCollectSingle->setEnabled(true);
    ui->actionCollectMulti->setEnabled(true);

    //    ui->actionSpaceAlgo->setEnabled(!inPreview);

}

void MainWindow::setNoCamera(){
    ui->actionPreview->setEnabled(false);
    ui->actionCollectSingle->setEnabled(false);
    ui->actionCollectMulti->setEnabled(false);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::Resize) {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        if(obj == ui->centralWidget){
                qDebug() << ui->srcVerticalLayout->sizeHint().width() << "高度" << ui->srcVerticalLayout->sizeHint().width();
                ui->srcVerticalLayout->setSizeConstraint(QLayout::SetMaximumSize);
                ui->srcVerticalLayout->setGeometry(QRect(0,0,(resizeEvent->size().width() - 30)/2,(resizeEvent->size().height()) - 24));
        }
     }
     return QWidget::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, APP_NAME,
                                                                tr("确认要退出吗?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        if(FullNameOfSelectedDevice.length() > 0){
            if(m_camera.IsGrabbing()){
                m_camera.StopGrabbing();
            }
            m_camera.Close();
            event->accept();
            exit(0);
        }
    }

}



void MainWindow::setting(){
    if(!(FullNameOfSelectedDevice.length() > 0)){
        QMessageBox::warning(this,APP_NAME,"请插入Basler相机",QMessageBox::Cancel,QMessageBox::Accepted);
        return;
    }
    qDebug() << "启动调试";
    SettingDialog *dialog = new SettingDialog(this);
    dialog->setSettings(saveSettings);
    int result = dialog->exec();
    if(result == QDialog::Accepted){
        saveSettings = dialog->getSaveSettings();
        startPreview();
    }
}

void MainWindow::about(){
    // AboutDialog *dialog = new AboutDialog(this);
    // dialog->exec();
}

void MainWindow::singleGrab(){
    if(m_camera.IsGrabbing()){
        m_camera.StopGrabbing();
    }

    CGrabResultPtr ptrGrabResult;
    CImageFormatConverter fc;
    fc.OutputPixelFormat = PixelType_BGR8packed;
    CPylonImage image;
    m_camera.StartGrabbing(1,GrabStrategy_LatestImageOnly);
    m_camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_Return);
    if (ptrGrabResult->GrabSucceeded())
    {

       fc.Convert(image, ptrGrabResult);
       cv::Mat openCvImage= cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) image.GetBuffer());
       cvtColor(openCvImage, openCvImage, cv::COLOR_BGR2RGB);

       QImage qtImage(openCvImage.data,openCvImage.cols,openCvImage.rows,openCvImage.step,QImage::Format_RGB888);
       ui->srcDisplay->setPixmap(QPixmap::fromImage(qtImage));
       ui->srcDisplay->update();
    }
    else
    {
        cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
    }

    m_camera.StopGrabbing();
    setButtons(false);


    QDir saveDir(saveSettings->FilePath);
    saveDir.mkdir("single");
    bool flag = true;
    int i = 0;
    while(flag){
        QString filePath = saveSettings->FilePath + "/single/" + saveSettings->FilePrefix + QString::number(i);
        if(saveSettings->format == ImageFileFormat_Tiff){
            filePath += ".tiff";
        }else if(saveSettings->format == ImageFileFormat_Png){
            filePath += ".png";
        }
        if(QFile::exists(filePath)){
            i++;
        }else{
            flag = false;
            ui->srcDisplay->pixmap(Qt::ReturnByValue).save(filePath);
        }
    }

}


void MainWindow::multiGrab(){
    if(m_camera.IsGrabbing()){
        m_camera.StopGrabbing();
    }
    SettingDialog *dialog = new SettingDialog(this);
    dialog->setSettings(saveSettings);
    int result = dialog->exec();
    if(result == QDialog::Accepted){
        saveSettings = dialog->getSaveSettings();

        try
         {
             INodeMap& nodemap = m_camera.GetNodeMap();

             CIntegerPtr Width (nodemap.GetNode("Width"));
             CIntegerPtr Height (nodemap.GetNode("Height"));
             CFloatPtr Rate( nodemap.GetNode("AcquisitionFrameRate") );
             //Acquisition frame rate of the camera in frames per second.
             CBooleanPtr AcquisitionFrameRateEnable( nodemap.GetNode("AcquisitionFrameRateEnable") );
             AcquisitionFrameRateEnable->FromString("true");
             CFloatPtr ExposureTime( nodemap.GetNode("ExposureTime") );


             Width->FromString(saveSettings->width);
             Height->FromString(saveSettings->height);
             ExposureTime->FromString(saveSettings->exposureTime);
             Rate->FromString(saveSettings->acquisitionFrameRate);
             //Exposure time of the camera in microseconds.

//             CImageFormatConverter fc;
//             fc.OutputPixelFormat = PixelType_BGR8packed;

             // This smart pointer will receive the grab result data.
             CGrabResultPtr ptrGrabResult;

             m_camera.StartGrabbing(GrabStrategy_LatestImageOnly);

             //创建目录
             QDir saveDir(saveSettings->FilePath);
             saveDir.mkdir("multi");

             QProgressDialog dialog;
             connect(&dialog,SIGNAL(canceled()),this,SLOT(startPreview()));
             dialog.setRange(0,saveSettings->image2save);
             dialog.setLabelText(QString("save %1 picture").arg(imageSaved));
             dialog.show();

             while(m_camera.IsGrabbing())
             {
                 m_camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

                 if (ptrGrabResult->GrabSucceeded())
                 {
                       //TODO 修改文件名
                       QString filePath = saveSettings->FilePath + "/multi/" + saveSettings->FilePrefix + QString::number(imageSaved);

                       if(saveSettings->format == ImageFileFormat_Tiff){
                           filePath += ".tiff";
                       }else if(saveSettings->format == ImageFileFormat_Png){
                           filePath += ".png";
                       }

                       CImagePersistence::Save( saveSettings->format, filePath.toUtf8().constData(), ptrGrabResult);
                       imageSaved++;
                       dialog.setLabelText(QString("save %1 picture").arg(imageSaved));
                       dialog.setValue(imageSaved);
                       if(imageSaved > saveSettings->image2save) {
                            m_camera.StopGrabbing();
                            imageSaved = 0;
                       }
                 }
                 else
                 {
                     cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
                 }
                 qApp->processEvents();

             }
         }
         catch (GenICam::GenericException &e){
            QMessageBox::warning(this,"",e.GetDescription(),QMessageBox::Cancel,QMessageBox::Accepted);
        }

        startPreview();

    }else{
        startPreview();
    }
}


void MainWindow::temporal_LSI(){

}

Mat MainWindow::spatial_LSI(Mat speckle,int m){

    speckle.convertTo(speckle, CV_32FC1);

    int n = m;

    Mat spatial_masker = Mat::ones(m,n,CV_32FC1)/(m*n);
    Mat resultSum;
    conv2(speckle,spatial_masker,CONVOLUTION_SAME,resultSum);

    Mat resultSqure;
    Mat speckeSqure;
    cv::pow(speckle,2,speckeSqure);

    conv2(speckeSqure,spatial_masker,CONVOLUTION_SAME,resultSqure);

    Mat tmp;
    cv::pow(resultSum,2,tmp);

    Mat trueResult;
    cv::divide((resultSqure - tmp)*1.0,tmp,trueResult);

    imshow("myWindow",abs( trueResult )*10);
    trueResult = abs( trueResult );
    return trueResult;

}


void MainWindow::conv2(const Mat &img, const Mat& kernel, ConvolutionType type, Mat& dest){
      Mat source = img;
      if(CONVOLUTION_FULL == type) {
        source = Mat();
        const int additionalRows = kernel.rows-1, additionalCols = kernel.cols-1;
        copyMakeBorder(img, source, (additionalRows+1)/2, additionalRows/2, (additionalCols+1)/2, additionalCols/2, BORDER_CONSTANT, Scalar(0));
      }

      Point anchor(kernel.cols - kernel.cols/2 - 1, kernel.rows - kernel.rows/2 - 1);
      int borderMode = BORDER_CONSTANT;
      Mat result;
      flip(kernel,result,-1);

      filter2D(source, dest, img.depth(), result, anchor, 0, borderMode);

      if(CONVOLUTION_VALID == type) {
        dest = dest.colRange((kernel.cols-1)/2, dest.cols - kernel.cols/2)
                   .rowRange((kernel.rows-1)/2, dest.rows - kernel.rows/2);
      }
}

void MainWindow::spatial_LSI_Matlab(){

    QString filePath = "/tmp/t.png";
    ui->srcDisplay->pixmap(Qt::ReturnByValue).save(filePath);

    Mat speckle;
    speckle = imread(filePath.toUtf8().constData(), cv::IMREAD_GRAYSCALE);
    speckle.convertTo(speckle, CV_32FC1);

    int m = 5;
    int n = m;
    Mat spatial_masker = Mat::ones(m,n,CV_32FC1)/(m*n);
    Mat resultSum;
    conv2(speckle,spatial_masker,ConvolutionType::CONVOLUTION_SAME,resultSum);
    Mat resultSqure;
    Mat speckeSqure;
    cv::pow(speckle,2,speckeSqure);
    conv2(speckeSqure,spatial_masker,ConvolutionType::CONVOLUTION_SAME,resultSqure);
    Mat tmp;
    cv::pow(resultSum,2,tmp);

    Mat trueResult;
    cv::divide((resultSqure - tmp)*1.0,tmp,trueResult);

    namedWindow( "Display window", WINDOW_NORMAL );// Create a window for display.
    imshow( "Display window", trueResult * 10 );                   // Show our image inside it.
    waitKey(0);


}

void MainWindow::algoArea(){
    const int deskW = QGuiApplication::primaryScreen()->geometry().width();
    if(isAlgoAreaOpened){
        isAlgoAreaOpened = false;
        ui->actionCloseAlgo->setText("关闭算法");

        int w = bigSize.width();

        setGeometry(
                     ceil((deskW - w)/2),
                     50,
                    w,
                    this->height()
                    );

        setFixedSize(w,this->height());
        ui->destDisplay->setHidden(false);

        int h = (ui->destDisplay->width() * atoi( saveSettings->height.c_str() ) / atoi( saveSettings->width.c_str()));
        ui->destDisplay->setGeometry(ui->destDisplay->geometry().x(),ui->srcDisplay->geometry().y(), ui->destDisplay->width(),h);
		ui->srcFrame->setGeometry(ui->srcFrame->geometry().x(), ui->srcFrame->geometry().y(), 658, 531);
		ui->srcHeader->resize(658, ui->srcHeader->size().height());
		ui->srcBottomhorizontalLayout->setGeometry(QRect(0, ui->srcBottomhorizontalLayout->geometry().y(), 658, ui->srcHeader->size().height()));
		ui->srcHeader->resize(658, ui->srcHeader->size().height());

    }else{
        isAlgoAreaOpened = true;
        ui->actionCloseAlgo->setText("打开算法");

        int w = smallSize.width() + 20;
        setGeometry(
                     ceil((deskW - w)/2),
                     50,
                    w,
                    this->height()
                    );

        setFixedSize(w,this->height());
        ui->destDisplay->setHidden(true);
		ui->srcFrame->setGeometry(ui->srcFrame->geometry().x(), ui->srcFrame->geometry().y(), 658, 531);
		ui->srcBottomhorizontalLayout->setGeometry(QRect(0, ui->srcBottomhorizontalLayout->geometry().y(), 658, ui->srcHeader->size().height()));
		ui->srcHeader->resize(658, ui->srcHeader->size().height());


    }
}

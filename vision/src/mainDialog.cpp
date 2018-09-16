#include "mainDialog.h"

MainDialog::MainDialog(){
	QSettings configuracion;

	menuPrincipal=new QMainWindow(this, Qt::Widget);
	menuPrincipal->addToolBar(new QToolBar());
	 resize(WW,WH);
	 //setFixedSize(WW,WH);
    ponerAcciones();
    ponerToolBar();
    librerias.leer((char *)vision::configuracion.cogerValor("dlls_temp").c_str());
    //librerias.ok();
    for(int i=0;i<librerias.numLibrerias;i++){
    	if(librerias.librerias[i]!=NULL){
    		if(strcmp(librerias.librerias[i]->getTipo(),"ALGORITMO")==0){
    			QString s(librerias.librerias[i]->getNombre());
    			algoritmos->addItem(s);
    		}

    	}
    }
    QObject::connect(algoritmos, SIGNAL(currentIndexChanged(int)),
                this, SLOT(seleccionarAlgoritmo(int)));

    QWidget * panel=new QWidget(this);
    panel->setStyleSheet("background-color:#000000");

    QHBoxLayout *divisionH=new QHBoxLayout();
    QVBoxLayout *divisionV=new QVBoxLayout();

    QFrame *frame=new QFrame();
    frame->setLayout(divisionH);

    imgViewer=new VisorWidget();
    divisionH->addWidget(imgViewer);
    divisionV->addWidget(frame);
    divisionV->addWidget(&log);
    qParametros=new QFrame();
    qParametros->setFixedWidth(250);
    qParametros->setStyleSheet("background-color:#F0F0F0");
    divisionH->addWidget(qParametros);
    panel->setLayout(divisionV);

    this->setCentralWidget(panel);


    int separadorAlto=5;
    //log.move(PL,top+PT+HTOTAL+separadorAlto);
    log.setFixedHeight(100);
    QPalette p=log.palette();
    p.setColor(QPalette::Background, Qt::black);
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    log.setPalette(p);
    //log.setParent(this);
    //log.show();

    resize(width(),top+PT+HTOTAL+(separadorAlto<<1)+HLOG);
    /*std::string rutaDll=vision::configuracion.cogerValor("dlls_temp");
    if(rutaDll!=""){
    	//Tenemos la ruta de ficheros, buscamos si hay elementos nuevos.
    	Log::consola(rutaDll);
    }/**/
    if(configuracion.contains("rutaFichero")){
          loadFile(configuracion.value("rutaFichero").toString());
       }

    //resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode){
    static bool firstDialog = true;
    if (firstDialog) {
        firstDialog = false;
        dialog.setDirectory( QDir::currentPath() );
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes){
        mimeTypeFilters.append(mimeTypeName);
    }
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave){
        dialog.setDefaultSuffix("jpg");
    }
}

void MainDialog::open(){
	QSettings configuracion;
	QString ruta=QDir::toNativeSeparators(configuracion.value("rutaFichero").toString());

    QFileDialog dialog(this, tr("Seleccionar fichero"),ruta);
    //initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    if (dialog.exec() == QDialog::Accepted){
    	if(loadFile(dialog.selectedFiles().first())){
    		QDir directorio(dialog.selectedFiles().first());
			configuracion.setValue("rutaFichero",directorio.absolutePath());
			configuracion.sync();

    	}
    }
}
void MainDialog::borrarMat(void *mat){
    delete static_cast<cv::Mat*>(mat);
}
bool MainDialog::loadFile(const QString &fileName){

	imagenCV = imread( fileName.toStdString());
	if ( !imagenCV.data ){
		QMessageBox mensaje;
		mensaje.setText("La imagen seleccionada no es válida");
		mensaje.setIcon(QMessageBox::Warning);
		mensaje.exec();
		return false;
	}
	cvtColor(imagenCV, imagenCV, COLOR_BGR2RGB);
	procesado=false;
	imagen=cv_qimage(imagenCV);
	std::cout<<imagen.rect().height()<<std::endl;
	imgViewer->setImage(imagen);
	//setImage(cv_qimage(imagenCV));

	//setWindowFilePath(fileName);

//	const QString message = tr("Abierto \"%1\", %2x%3, Depth: %4")
	//	.arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
	//statusBar()->showMessage(message);
	return true;
	/*namedWindow("Display Image", WINDOW_AUTOSIZE );
	imshow("Display Image", image);
	waitKey(0);




	QImageReader reader(fileName);

    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);

	setWindowFilePath(fileName);

	const QString message = tr("Abierto \"%1\", %2x%3, Depth: %4")
		.arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
	statusBar()->showMessage(message);
	return true;*/
}
void MainDialog::setImage(const QImage &newImage){
   return;
   iCVW[0]=newImage.width();
	iCVH[0]=newImage.height();
	int w=iCVW[0];
	int h=iCVH[0];
	int wTratar=(procesado?WP:W);
	int hTratar=(procesado?HP:H);
	if(iCVW[0]>wTratar){
		w=wTratar;
	}
	if(iCVH[0]>hTratar){
		h=hTratar;
	}
	imagen = newImage.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	iW[0]=imagen.width();
	iH[0]=imagen.height();
	posX[0]=(wTratar-iW[0])>>1; //Buscamos el medio en Ancho
	posY[0]=(hTratar-iH[0])>>1; //Buscamos el medio en Alto
	//imageLabel->setPixmap(QPixmap::fromImage(image));
	//scrollArea->setVisible(true);
	//imageLabel->adjustSize();
	//cambio=true;
	qDebug()<<"actualizao con posY="<<posY[0]<<" h="<<h<<" iH="<<iH[0]<<"\r\n";
	this->update();
}
void MainDialog::setImagenTratada(const QImage &imagenTemp){
   //imagenTratada=
	iCVW[1]=imagenTemp.width();
	iCVH[1]=imagenTemp.height();
	int w=iCVW[1];
	int h=iCVH[1];

	if(iCVW[1]>W){
		w=W;
	}
	if(iCVH[1]>H){
		h=H;
	}

	//imagenTratada = new QImage();
	//imagenTratada=imagenTemp.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	imagenTratada=imagenTemp;
	setImage(cv_qimage(imagenCV));
	iW[1]=imagenTratada.width();
	iH[1]=imagenTratada.height();
	posX[1]=(W-iW[1])>>1; //Buscamos el medio en Ancho
	posY[1]=(H-iH[1])>>1; //Buscamos el medio en Alto

	this->update();
}
QImage MainDialog::cv_qimage(const cv::Mat &cvMat){

	const QImage imageTemp((const unsigned char*)cvMat.data, cvMat.cols, cvMat.rows, cvMat.step,
		                       QImage::Format_RGB888, &borrarMat, new Mat(cvMat));
	imageTemp.rgbSwapped();
	return imageTemp;
}
QImage MainDialog::cv_qimageGris(const cv::Mat &cvMat){

	const QImage imageTemp((const unsigned char*)cvMat.data, cvMat.cols, cvMat.rows, cvMat.step,
		                       QImage::Format_Grayscale8, &borrarMat, new Mat(cvMat));
	//imageTemp.rgbSwapped();
	return imageTemp;
}
void MainDialog::paintEvent(QPaintEvent *){
   return;
	int top=this->top+PT;
	int left=PL;
	int w=WTOTAL+4; //Sumamos 1 de la l�nea, 1 del margen interno (padding) y eso por cada lado =4
	int h=HTOTAL+4;
	QPainter p(this);
	p.fillRect(PL,top,w,h,Qt::darkGray);
	p.drawRect(PL,top,w,h);
	//Ahora tenemos que centrar
	//TODO: Acelarar guardando en variables y solo cambiandolas cuando se cambie la imagen

	//p.drawImage(PL+2+posX[0],top+2+posY[0], imagen); //Movemos dos tanto arriba como abajo para el borde + el padding
	//if(imagenTratada!=NULL){

//	}else{
		p.drawImage(PL+2+posX[0],top+2+posY[0], imagen);
		if(procesado){
			p.drawImage(PL+2+posX[1],((top+2+posY[1]))+HP+2, imagenTratada);
		}
	//}
}
void MainDialog::ejecutarHilo(cv::Mat * img){
   boost::shared_ptr<vision::Plugin> p=librerias.librerias[algoritmos->currentIndex()-1];
   if(p->procesar(imagenCV)){
      *img=(p->getValor<cv::Mat>("imagen"));
   }else{
      *img= NULL;
   }
	//*img=;
}
void MainDialog::ejecutarMetodo(){
	if(algoritmos->currentIndex()>0){
		try{
			std::string logProceso;
			boost::shared_ptr<vision::Plugin> plugin=librerias.librerias[algoritmos->currentIndex()-1];
			if(plugin){
				log.clear();
				QString salida=QStringLiteral("Comenzamos con el método ");
				salida.append(plugin->getNombre());
				salida.append("\r\n");
				log.setText(salida);
				plugin->setDebug(&logProceso);
				std::clock_t c_start = std::clock();
				cv::Mat imagenCVTemp;
				boost::thread t=boost::thread(&MainDialog::ejecutarHilo,this, &imagenCVTemp);
				t.join();
				std::clock_t c_end = std::clock();
				//std::chrono::duration<double, std::milli>(t_end-t_start).count()
				//auto t_end = std::chrono::high_resolution_clock::now();
				/*const char * ll=plugin->getLog();
				qDebug()<<ll<<"\r\n";
				qDebug()<<logProceso.c_str()<<"\r\n";*/
				log.insertPlainText(QString::fromStdString(logProceso));
				log.insertPlainText(QString("Tardo "));
				log.insertPlainText(QString::number((1000.0 * (c_end-c_start) / CLOCKS_PER_SEC)));
				//log.append(QString::number(((c_end-c_start))));
				log.insertPlainText(QString(" ms\r\n"));
				//std::cout<<"Tengo la imagen y la devuelvo"<<imagenCVTemp.rows<<" \r\n";
				procesado=true;
				if(imagenCVTemp.channels()>1){
					imagenTratada=cv_qimage(imagenCVTemp);
				}else{
				   imagenTratada=cv_qimageGris(imagenCVTemp);
				}
				imgViewer->setImage(imagenTratada);
			}
		}catch(...){ // @suppress("Catching by reference is recommended")
			qDebug()<<"Se ha producido un error a la hora de llamar al método.";
		};
	}else{
		qDebug()<<"Seleccionado el 0";
	}
	/*//Recorremos cada byte
	cv::Mat *imagen= &(this->imagenCV);
	qDebug()<<imagen->depth()<<"-"<<imagen->channels();
	String s="";
	int bits=0;
	unsigned char caracter=0;
	for(int x=0; x<imagen->cols; x++){
		for(int y=0; y<imagen->rows; y++){
			cv::Vec3b pixel = imagen->at<cv::Vec3b>(cv::Point(x,y));
			bool proceso=true;
			for(int i=0;i<3 && proceso;i++){


				caracter=(caracter<<1)|(pixel.val[i]&1);

				if(bits==7){
					//std::cout<<(char)caracter;
					bits=0;
					caracter=0;
					proceso=false;
				}else{
					bits++;
				}

			}



		}
	}
	std::cout<<"\r\n";
	qDebug()<<"Termino";/**/
}

void MainDialog::ponerAcciones(){
    QMenu *fileMenu = menuBar()->addMenu(tr("&Abrir"));

    QAction *openAct = fileMenu->addAction(tr("&Abrir..."), this, &MainDialog::open);
    openAct->setShortcut(QKeySequence::Open);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("&Salir"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));
    top+=menuBar()->height();

}
void MainDialog::ponerToolBar(){

	//QMainWindow * mainWindow = new QMainWindow();
	QToolBar * toolBar = new QToolBar(tr("ejecucion"));
	toolBar->setAllowedAreas(Qt::TopToolBarArea);
	toolBar->setMovable(false);

	algoritmos=new QComboBox();
	algoritmos->addItem("__ALGORITMOS__");
	toolBar->addWidget(algoritmos);/**/

	QPushButton *ejecutar=new QPushButton(QIcon(":/ejecutar.png"),"Ejecutar");
	//ejecutar->setEnabled(false);
	connect(ejecutar, SIGNAL (released()),this, SLOT (ejecutarMetodo()));
	toolBar->addWidget(ejecutar);/**/
	QPushButton *guardar=new QPushButton(QIcon(":/ejecutar.png"),"Guardar");
	connect(guardar, SIGNAL (released()),this, SLOT (guardarImagen()));
	toolBar->addWidget(guardar);


	addToolBar(toolBar);
	top+=20;


}

void MainDialog::guardarImagen() {
   QString filename = QFileDialog::getSaveFileName(this, tr("Guardar imagen"),"untitled.png",
                              tr("Images (*.jpg *.png *.xpm)"));
   imagenTratada.save(filename);
}


void MainDialog::seleccionarAlgoritmo(int indice){
   if(qParametros->layout()!=0){
      QLayout* layout = qParametros->layout ();
      QLayoutItem *item;
      while ((item = layout->takeAt(0)) != 0){
         layout->removeItem (item);
      }
      delete layout;
   }
   qParametros->update();
   if(indice!=0){
      qDebug()<<"Preparamos la zona de configuración: "<<QString::number(indice);

      QVBoxLayout *lV=new QVBoxLayout();
      lV->setAlignment(Qt::AlignTop);
      qParametros->setLayout(lV);
      boost::shared_ptr<vision::Plugin> plugin=librerias.librerias[algoritmos->currentIndex()-1];
      if(plugin){
         std::vector<std::pair<std::string,vision::Plugin::TIPO>> parametros=plugin->getParametros();
         //qDebug()<<"hay "<< parametros.size()<<" parametros";
         for(std::vector<std::pair<std::string,vision::Plugin::TIPO>>::iterator it=parametros.begin();it!=parametros.end();++it){
            std::string parametro= std::get<0>(*it);
            switch(((int)std::get<1>(*it))){
               case vision::Plugin::TIPO::FUNCTION:
                  {
                     QPushButton *b=new QPushButton();

                     b->setText(parametro.c_str());
                     lV->addWidget(b);
                     connect(b, &QPushButton::clicked, [plugin, parametro](){
                        //boost::shared_ptr<vision::Plugin> plugin=this->librerias.librerias[algoritmos->currentIndex()-1];
                        void *f=plugin->getParametro<void*>(parametro);
                        if(f==0){
                           qDebug()<<"No hay acción";
                        }else{
                           qDebug()<<"He pulsado el boton de "<<&f;
                        }

                     });

                  }
                  break;
               default:
                  {
                     QLabel *l=new QLabel(parametro.c_str());
                     lV->addWidget(l);
                     QLineEdit * e=new QLineEdit();
                     lV->addWidget(e);
                     //qDebug()<<"Busco parametro "<<parametro.c_str();
                     std::string s=plugin->getParametro<std::string>(parametro);
                     //qDebug()<<"encuentro parametro "<<parametro.c_str();
                     e->setText(s.c_str());
                     connect(e, &QLineEdit::textChanged, [plugin, parametro](const QString & texto){
                        plugin->setParametro<std::string>(parametro,std::string(texto.trimmed().toUtf8().constData()));
                        //qDebug()<<"Nuevo texto "<<texto;

                     });
                     //((s!=NULL)?"SI":"NULO");
                     /*if(s!=0){
                        qDebug()<<"NOes nulo parametro "<<parametro.c_str();
                        //      qDebug()<<" y vale "<<&s;

                     }else{
                        qDebug()<<"es nulo parametro "<<parametro.c_str();
                     }*/
                  }
                  break;

            }
            //qDebug()<<((std::string)(std::get<0>(*it))).c_str()<<" de tipo "<<((int)std::get<1>(*it));
         }
      }
      //panel->cargarImagen(configuracion.value("ultima_imagen_"+QString::number(indice)).toString());
   }
}

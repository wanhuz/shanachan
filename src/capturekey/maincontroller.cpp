#include "../capturekey/maincontroller.h"
#include "../capturekey/capturekey.h"
#include "../ui/frame.h"
#include "../util/util.h"
#include "../dict/DictLoader.h"
#include "../dict/dict.h"
#include <qtimer.h>
#include <qdebug.h>
#include <qclipboard.h>
#include <qapplication.h>
#include "../settings/config.h"
#include "../util/util.h"


MainController::MainController() {
	DictLoader* dictloader = new DictLoader();
	dict = new Dict();
	frame = new Frame();
	ocr = new Ocr();
	capturekeypress = new capturekey();
	clipboard = QApplication::clipboard();
	Config::getInstance().setFrame(frame);
	dictloader->setDict(dict);
	dictloader->start();
	
	connect(capturekeypress, SIGNAL(OCRkeyStateChanged()), this, SLOT(captureOCR()));
	connect(capturekeypress, SIGNAL(TextkeyStateChanged()), this, SLOT(captureTextGeneric()));
}

void MainController::killCaptureKey() {
	capturekeypress->stopCapture(capturekeypress->OCR);
	capturekeypress->stopCapture(capturekeypress->TEXT_GENERIC);
	capturekeypress->quit();
	capturekeypress->wait();
}

void MainController::stopCaptureKey() {
	disconnect(clipboard, SIGNAL(dataChanged()), this, SLOT(setSearchBox()));
	capturekeypress->stopCapture(capturekeypress->OCR);
	capturekeypress->stopCapture(capturekeypress->TEXT_GENERIC);
}

void MainController::startCaptureKeyOCR() {
	this->stopCaptureKey();
	capturekeypress->startCapture(capturekeypress->OCR);

	if (!capturekeypress->isRunning()) {
		capturekeypress->start();
	}

}

void MainController::startCaptureKeyTextGeneric() {
	this->stopCaptureKey();
	connect(clipboard, SIGNAL(dataChanged()), this, SLOT(setSearchBox()));
	capturekeypress->startCapture(capturekeypress->TEXT_GENERIC);

	if (!capturekeypress->isRunning()) {
		capturekeypress->start();
	}
	
}

QVector<entry> MainController::searchDict(QString searchStr) {
	QVector<entry> searchResult;
	QString rootWord = Util::getRootWord(searchStr);

	if ( (rootWord.isEmpty()) || (rootWord == searchStr) ) {
		searchResult = dict->search(searchStr);
		searchResult = dict->sort(searchResult, searchStr);
	}
	else {
		searchResult = dict->searchWithRoot(searchStr, rootWord);
		searchResult = dict->sortWithRoot(searchResult, searchStr, rootWord);
	}
	
	
	return searchResult;
}

void MainController::captureOCR() {
	frame->setBoxSize();
	QPixmap screenshot = frame->shootScreenshot();
	QTimer *timer = new QTimer();
	timer->start(2500);
	frame->show();
	frame->activateWindow();
	Pix *pix = Util::qPixMap2PIX(&screenshot);
	QString text = ocr->recognize(pix);

	//Remove space get correct result
	text = text.simplified();
	text = text.replace(" ", "");

	emit OcrResult(text);

	connect(timer, &QTimer::timeout, frame, &Frame::hide);
	connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
}


/*Capture digital text by sending CTRL+C as input to Windows
It is a hacky solution, but it is the most optimal way to get text from different application types in Microsoft Windows*/
void MainController::captureTextGeneric() {
	Util::sendKeyInput();
}

/*If text data changed in clipboard, send it to MainWindow's textbox*/
void MainController::setSearchBox() {
	QString clipText = clipboard->text();
	emit OcrResult(clipText);
}
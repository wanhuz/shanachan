#include "popup.h"
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qdebug.h>
#include <qcursor.h>
#include <qsizepolicy.h>
#include "../data/entry.h"
#include <qdir.h>
#include <qfontdatabase.h>

#define MAX_ENTRY 5
#define MIN_ENTRY 0


popup::popup(QWidget* parent)
	: QDialog(parent)
{
	
	QBoxLayout* hlayoutbottom = new QHBoxLayout;
	QPushButton* btnPrev = new QPushButton("^");
	QPushButton* btnNext = new QPushButton("v");
	QBoxLayout* vlayout = new QVBoxLayout;

	//Init theme
	this->setTheme();
	
	//Set widget properties
	this->setWindowFlags(Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::Popup);
	this->setContentsMargins(0, 0, 0, 0);
	this->setLayout(vlayout);
	gloss->setWordWrap(true);
	gloss->setAlignment(Qt::AlignJustify);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	gloss->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setMinimumHeight(140);

	//Set layout properties
	btnPrev->setFixedSize(25, 25);
	btnNext->setFixedSize(25, 25);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	vlayout->addWidget(kanji);
	vlayout->addWidget(kana);
	hlayoutbottom->addStretch();
	hlayoutbottom->addWidget(btnPrev);
	hlayoutbottom->addSpacing(5);
	hlayoutbottom->addWidget(btnNext);
	hlayoutbottom->setContentsMargins(5, 5, 5, 5);
	vlayout->addWidget(gloss);
	vlayout->addLayout(hlayoutbottom);
	
	connect(btnNext, &QPushButton::clicked, this, [=]() {if (ind < entsize-1) { ind++; }});
	connect(btnPrev, &QPushButton::clicked, this, [=]() {if (ind > MIN_ENTRY) { ind--; }});
	connect(btnNext, SIGNAL(clicked()), this, SLOT(selectEntry()));
	connect(btnPrev, SIGNAL(clicked()), this, SLOT(selectEntry()));
}


void popup::addEntry(entry ent) {
	if (topEntry.size() > MAX_ENTRY) {
		printf("Entry is higher than 5. No new entry is added");
		return;
	}

	entsize++;
	entry newEnt = this->processEnt(ent);
	topEntry.append(newEnt);
}

void popup::clearEntry() {
	if (topEntry.size() > 0) {
		topEntry.clear();
	}

	entsize = 0;
	ind = 0;
}

void popup::selectEntry() {
	if (topEntry.size() == 0 || topEntry.isEmpty()) { 
		printf("No entry"); 
		return; 
	}

	kanji->setText(topEntry[ind].getKanji());
	kana->setText(topEntry[ind].getReading());
	gloss->setText(topEntry[ind].getGloss());

	this->setGeometry(x, y, 224, 200);
}

//Select the first entry before showing the widget
void popup::shows() {
	QPoint curCurPos = QCursor::pos();
	x = curCurPos.x() + 10;
	y = curCurPos.y() - (200 + 10);


	this->selectEntry();
	this->show();
}

void popup::setTheme() {
	const QString style("QDialog {background-color: white; border: 1px solid black;}"
						"QPushButton {border:none; background-color: #3f87f5; color: white;}" 
						"QPushButton:hover {background-color: #3867db}");

	QString fontPath = QDir::currentPath();
	fontPath = "C:\\Users\\WanHuz\\Documents\\Shanachan\\res\\font\\NotoSansMonoCJKjp-Regular.otf"; //For debugging purpose
	//fontPath = fontPath + "//res//NotoSansMonoCJKjp-Regular.otf";
	int id = QFontDatabase::addApplicationFont(fontPath);

	if (id < 0) {
		qDebug() << "Failed to load Sans Mono JK font at " << fontPath;
	}
	else {
		QString NotoJK = QFontDatabase::applicationFontFamilies(id).at(0);
		sansMonoJK = new QFont(NotoJK, 12);
		kanji->setFont(*sansMonoJK);
		kana->setFont(*sansMonoJK);
	}

	kanji->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	kana->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	kanji->setContentsMargins(11, 5, 5, 5);
	kana->setContentsMargins(11, 5, 5, 5);
	gloss->setContentsMargins(11, 11, 11, 11);

	kanji->setStyleSheet("font-size: 20px; font-weight: bold; background-color: #3867db; color: white;");
	kana->setStyleSheet("font-size: 15px; font-weight: bold; background-color: #3f87f5; color: white;");
	this->setStyleSheet(style);
}

//Process entry
entry popup::processEnt(entry ent) {

	//Only take one kana reading
	QString kana = ent.getReading();
	int kanaind = kana.indexOf("\n");
	if (kanaind > -1) {
		kana.remove(kanaind, kana.size());
	}

	//Only take first N string of glossary
	QString gloss = ent.getGloss();
	int n = 200;
	if (gloss.size() > n) {
		gloss.remove(n, gloss.size());
		gloss.append(" ... (more)");
	}

	//If there is no kanji entry, put kana in kanji
	QString kanji = ent.getKanji();
	if (kanji.isEmpty()) {
		kanji = kana;
		kana = "";
	}
	
	entry tempEnt(
		kanji,
		kana,
		gloss,
		ent.getFreq()
	);

	return tempEnt;


}
#include "TubeWidget.h"
#include "GuiDefs.h"

#include <sstream>


TubeWidget::TubeWidget(QWidget *parent, std::string hostname)
    :   QWidget(parent), hostname(hostname) {
	setupUi(this);
    LayoutWindow();
    Initialization();
}

TubeWidget::~TubeWidget() {}

void TubeWidget::LayoutWindow() {
}

void TubeWidget::Initialization() {
}

void TubeWidget::Update() {
}


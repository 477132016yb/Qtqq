#include "CommonUtils.h"

CommonUtils::CommonUtils()
{
}

QPixmap CommonUtils::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0)) {
		maskSize = mask.size();
	}
	else{
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	QImage reultImage(maskSize, QImage::Format_ARGB32_Premultiplied);//保存转换后的头像
	QPainter painter(&reultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(reultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(reultImage);
}

void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		widget->setStyleSheet("");
		QString qsstyleSheet= QLatin1String(file.readAll());
	}
	file.close();
}

void CommonUtils::setDefaultSkinColor(const QColor& color)
{
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	QSettings settngs(path, QSettings::IniFormat);
	settngs.setValue("DefaultSkin/red", color.red());
	settngs.setValue("DefaultSkin/green", color.green());
	settngs.setValue("DefaultSkin/blue", color.blue());
}

QColor CommonUtils::getDefaultSkinColor()
{
	QColor color;
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	if (!QFile::exists(path)) {
		setDefaultSkinColor(QColor(22, 154, 88));
	}
	QSettings settngs(path, QSettings::IniFormat);
	color.setRed(settngs.value("DefaultSkin/red").toInt());
	color.setGreen(settngs.value("DefaultSkin/green").toInt());
	color.setBlue(settngs.value("DefaultSkin/blue").toInt());
	return color;
}

CustomProxyStyle::CustomProxyStyle()
{
}

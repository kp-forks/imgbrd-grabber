#ifndef EXTENSION_ROTATOR_H
#define EXTENSION_ROTATOR_H

#include <QObject>
#include <QString>
#include <QStringList>


class ExtensionRotator : public QObject
{
	Q_OBJECT

	public:
		ExtensionRotator() = default;
		ExtensionRotator(const ExtensionRotator &other);
		explicit ExtensionRotator(const QString &initialExtension, const QStringList &extensions, QObject *parent = nullptr);
		QString next();

	private:
		int m_initialExtensionIndex;
		QStringList m_extensions;
		int m_next = 0;
};

#endif // EXTENSION_ROTATOR_H

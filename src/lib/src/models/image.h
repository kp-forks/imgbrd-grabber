#ifndef IMAGE_H
#define IMAGE_H

#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QString>
#include "image-size.h"
#include "loader/downloadable.h"
#include "loader/token.h"
#include "models/pool.h"
#include "tags/tag.h"


class ExtensionRotator;
class NetworkReply;
class Page;
class Profile;
class QDateTime;
class QPixmap;
class QSettings;
class Site;

class Image : public QObject, public Downloadable
{
	Q_OBJECT

	public:
		enum LoadTagsResult
		{
			Ok,
			Error,
			CloudflareError,
			NetworkError
		};
		Q_ENUM(LoadTagsResult)

		Q_ENUM(SaveResult)
		Q_ENUM(Size)

		Image();
		explicit Image(Profile *profile);
		Image(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent = nullptr);
		Image(Site *site, QMap<QString, QString> details, QVariantMap identity, QVariantMap data, Profile *profile, Page *parent = nullptr);
		Image(const Image &other);

		// Serialization
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, const QMap<QString, Site*> &sites);

		int value() const;
		QString md5() const;
		const QList<Tag> &tags() const;
		QStringList tagsString(bool namespaces = false) const;
		const QList<Pool> &pools() const;
		qulonglong id() const;
		QVariantMap identity(bool id = false) const;
		int fileSize() const;
		int width() const;
		int height() const;
		const QStringList &search() const;
		QDateTime createdAt() const;
		QString dateRaw() const;
		const QUrl &pageUrl() const;
		const QUrl &fileUrl() const;
		QSize size(Size size = Size::Full) const;
		QRect rect(Size size = Size::Full) const;
		const QString &name() const;
		Page *page() const;
		const QUrl &parentUrl() const;
		Site *parentSite() const;
		ExtensionRotator *extensionRotator() const;
		bool hasTag(QString tag) const;
		bool hasUnknownTag() const;
		void setUrl(const QUrl &url);
		void setSize(QSize size, Size s);
		void setFileSize(int fileSize, Size s);
		void setFileExtension(const QString &ext);
		void setTemporaryPath(const QString &path, Size size = Size::Full);
		void setSavePath(const QString &path, Size size = Size::Full);
		QString savePath(Size size = Size::Full) const;
		Size preferredDisplaySize() const;
		bool isVideo() const;
		QString isAnimated() const;
		void setTags(const QList<Tag> &tags);
		bool isGallery() const;
		QString extension() const;
		void setParentGallery(const QSharedPointer<Image> &parentGallery);
		void setPromoteDetailParsWarn(bool);
		bool isValid() const;
		Profile *getProfile() const { return m_profile; }
		const ImageSize &mediaForSize(const QSize &size, bool thumbnail = false);
		QList<QPair<QString, int>> ugoiraFrameInformation() const;

		// Preview pixmap store
		QPixmap previewImage() const;
		const QPixmap &previewImage();
		void setPreviewImage(const QPixmap &preview);

		// Displayable
		QColor color() const override;
		QString tooltip() const override;
		QString counter() const override;
		QList<QStrP> detailsData() const override;

		// Downloadable
		QUrl url(Size size = Size::Full) const override;
		void preload(const Filename &filename) override;
		QStringList paths(const QString &filename, const QString &folder, int count) const;
		QStringList paths(const Filename &filename, const QString &folder, int count) const override;
		QMap<QString, Token> generateTokens(Profile *profile) const override;
		SaveResult preSave(const QString &path, Size size) override;
		QString postSave(const QString &path, Size size, SaveResult result, bool addMd5, bool startCommands, int count, bool basic = false) override;

		// Tokens
		template <typename T>
		T token(const QString &name, const T &defaultValue = T()) const
		{
			const QMap<QString, Token> &toks = tokens(m_profile);
			if (!toks.contains(name)) {
				return defaultValue;
			}
			return toks[name].value<T>();
		}

	protected:
		void init();
		QString md5forced() const;
		QString postSaving(const QString &path, Size size, bool addMd5 = true, bool startCommands = false, int count = 1, bool basic = false);

	public slots:
		void loadDetails(bool rateLimit = false);
		void abortTags();
		void parseDetails();
		void parseUgoiraDetails();

	signals:
		void finishedLoadingPreview();
		void finishedLoadingTags(Image::LoadTagsResult result);
		void urlChanged(const QUrl &before, const QUrl &after);

	private:
		Profile *m_profile;
		Page *m_parent = nullptr;
		QUrl m_url;
		QString m_extension;
		QUrl m_pageUrl;
		QUrl m_parentUrl;
		QSettings *m_settings;
		Site *m_parentSite;
		bool m_loadingDetails = false;
		bool m_loadedDetails = false;
		bool m_detailsParsWarnAsErr = false;

		// Shared
		// - Technical
		int m_position;
		QStringList m_search;
		// - Data
		QVariantMap m_identity;
		qulonglong m_id;
		QVariantMap m_data;
		bool m_isGallery = false;

		// Image
		// - Technical
		ExtensionRotator *m_extensionRotator;
		NetworkReply *m_loadDetails = nullptr;
		// - Data
		QString mutable m_md5;
		QSharedPointer<Image> m_parentGallery;
		QMap<Image::Size, QSharedPointer<ImageSize>> m_sizes;
		QList<QSharedPointer<ImageSize>> m_allSizes;
		QList<Tag> m_tags;
		QList<Pool> m_pools;
		QStringList m_sources;

		// Gallery
		// - Data
		int m_galleryCount;
		QString m_name;
};

Q_DECLARE_METATYPE(Image)

#endif // IMAGE_H

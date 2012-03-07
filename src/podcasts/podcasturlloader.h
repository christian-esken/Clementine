/* This file is part of Clementine.
   Copyright 2012, David Sansome <me@davidsansome.com>
   
   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PODCASTURLLOADER_H
#define PODCASTURLLOADER_H

#include <QObject>
#include <QRegExp>

#include "podcast.h"

class PodcastParser;

class QNetworkAccessManager;
class QNetworkReply;

class PodcastUrlLoaderReply : public QObject {
  Q_OBJECT

public:
  PodcastUrlLoaderReply(const QUrl& url, QObject* parent);

  const QUrl& url() const { return url_; }
  bool is_finished() const { return finished_; }
  bool is_success() const { return error_text_.isEmpty(); }
  const QString& error_text() const { return error_text_; }
  const PodcastList& results() const { return results_; }

  void SetFinished(const QString& error_text);
  void SetFinished(const PodcastList& results);

signals:
  void Finished(bool success);

private:
  QUrl url_;
  bool finished_;
  QString error_text_;
  PodcastList results_;
};


class PodcastUrlLoader : public QObject {
  Q_OBJECT

public:
  PodcastUrlLoader(QObject* parent = 0);
  ~PodcastUrlLoader();

  static const int kMaxRedirects;

  PodcastUrlLoaderReply* Load(const QString& url_text);

private:
  struct RequestState {
    int redirects_remaining_;
    PodcastUrlLoaderReply* reply_;
  };

  typedef QPair<QString, QString> QuickPrefix;
  typedef QList<QuickPrefix> QuickPrefixList;

private slots:
  void RequestFinished(RequestState* state, QNetworkReply* reply);

private:
  QUrl FixPodcastUrl(const QString& url_text) const;
  void SendErrorAndDelete(const QString& error_text, RequestState* state);
  void NextRequest(const QUrl& url, RequestState* state);

private:
  QNetworkAccessManager* network_;
  PodcastParser* parser_;

  QuickPrefixList quick_prefixes_;

  QRegExp html_link_re_;
  QRegExp whitespace_re_;
  QRegExp html_link_rel_re_;
  QRegExp html_link_type_re_;
  QRegExp html_link_href_re_;
};

#endif // PODCASTURLLOADER_H
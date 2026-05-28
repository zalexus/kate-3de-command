#include <ktexteditor/plugin.h>
#include <ktexteditor/mainwindow.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <KPluginFactory>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QMap>
#include <QIcon>
#include <QDebug>

class DictPlugin : public KTextEditor::Plugin
{
    Q_OBJECT
public:
    explicit DictPlugin(QObject* parent = nullptr, const QVariantList& args = QVariantList())
        : KTextEditor::Plugin(parent) {}
    
    QObject* createView(KTextEditor::MainWindow* mainWindow) override;
};

class DictPluginView : public QObject
{
    Q_OBJECT
public:
    explicit DictPluginView(DictPlugin* plugin, KTextEditor::MainWindow* mainWindow)
        : QObject(mainWindow)
        , m_mainWindow(mainWindow)
    {
        // Инициализация словаря из dictionary.h
        QMap<QString, QString> dict;
        #include "dictionary.h"
        m_dict = dict;
        
        qDebug() << "Dictionary loaded, size:" << m_dict.size();
        
        QWidget* tv = mainWindow->createToolView(
            plugin,
            QStringLiteral("dict_panel"),
            KTextEditor::MainWindow::Left,
            QIcon::fromTheme(QStringLiteral("accessories-dictionary")),
            QStringLiteral("Dictionary")
        );
        
        tv->setMinimumWidth(320);
        
        QWidget* container = new QWidget(tv);
        QVBoxLayout* mainLayout = new QVBoxLayout(container);
        mainLayout->setContentsMargins(8, 8, 8, 8);
        
        m_searchEdit = new QLineEdit(container);
        m_searchEdit->setPlaceholderText("🔍 Enter search mask...");
        m_searchEdit->setClearButtonEnabled(true);
        mainLayout->addWidget(m_searchEdit);
        
        m_listWidget = new QListWidget(container);
        m_listWidget->setAlternatingRowColors(true);
        mainLayout->addWidget(m_listWidget);
        
        m_textEdit = new QTextEdit(container);
        m_textEdit->setReadOnly(true);
        m_textEdit->setPlaceholderText("Select a word to see description...");
        m_textEdit->setMinimumHeight(150);
        mainLayout->addWidget(m_textEdit);
        
        connect(m_searchEdit, &QLineEdit::textChanged, this, &DictPluginView::updateList);
        connect(m_listWidget, &QListWidget::currentTextChanged, this, &DictPluginView::updateText);
        
        // Двойной клик по элементу списка
        connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &DictPluginView::insertWord);
        
        updateList();
        
        if (tv->layout()) {
            tv->layout()->addWidget(container);
        } else {
            QVBoxLayout* rootLayout = new QVBoxLayout(tv);
            rootLayout->setContentsMargins(0, 0, 0, 0);
            rootLayout->addWidget(container);
        }
    }
    
private slots:
    void updateList(const QString& mask = QString())
    {
        if (!m_listWidget) return;
        
        m_listWidget->clear();
        
        QString searchMask = mask.isEmpty() ? m_searchEdit->text() : mask;
        
        for (auto it = m_dict.begin(); it != m_dict.end(); ++it) {
            if (searchMask.isEmpty() || it.key().contains(searchMask, Qt::CaseInsensitive)) {
                m_listWidget->addItem(it.key());
            }
        }
        
        if (m_listWidget->count() > 0) {
            m_listWidget->setCurrentRow(0);
        } else {
            m_textEdit->setText("📭 No matches found.\n\nTry a different search mask.");
        }
    }
    
    void updateText(const QString& key)
    {
        if (!m_textEdit || key.isEmpty()) return;
        
        if (m_dict.contains(key)) {
            QString text = m_dict[key];
            text.prepend("<b>📖 " + key.toUpper() + "</b><br><br>");
            m_textEdit->setHtml(text);
        } else {
            m_textEdit->clear();
        }
    }
    
    void insertWord(QListWidgetItem* item)
    {
        if (!item) return;
        
        QString word = item->text();
        
        // Получаем активный документ
        KTextEditor::View* view = m_mainWindow->activeView();
        if (!view) {
            qDebug() << "No active view";
            return;
        }
        
        KTextEditor::Document* doc = view->document();
        if (!doc) {
            qDebug() << "No active document";
            return;
        }
        
        // Вставляем слово в текущую позицию курсора
        KTextEditor::Cursor cursor = view->cursorPosition();
        doc->insertText(cursor, word);
        
        qDebug() << "Inserted word:" << word;
    }
    
private:
    QLineEdit* m_searchEdit = nullptr;
    QListWidget* m_listWidget = nullptr;
    QTextEdit* m_textEdit = nullptr;
    QMap<QString, QString> m_dict;
    KTextEditor::MainWindow* m_mainWindow = nullptr;
};

QObject* DictPlugin::createView(KTextEditor::MainWindow* mainWindow)
{
    return new DictPluginView(this, mainWindow);
}

K_PLUGIN_FACTORY_WITH_JSON(DictPluginFactory, "3decommand.json", registerPlugin<DictPlugin>();)

#include "plugin.moc"

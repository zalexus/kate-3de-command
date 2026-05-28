#include <ktexteditor/plugin.h>
#include <ktexteditor/mainwindow.h>
#include <KPluginFactory>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>

class TestPlugin : public KTextEditor::Plugin
{
    Q_OBJECT
public:
    explicit TestPlugin(QObject* parent = nullptr, const QVariantList& args = QVariantList())
        : KTextEditor::Plugin(parent) {}

    QObject* createView(KTextEditor::MainWindow* mainWindow) override;
};

class TestPluginView : public QObject
{
    Q_OBJECT
public:
    explicit TestPluginView(TestPlugin* plugin, KTextEditor::MainWindow* mainWindow)
        : QObject(mainWindow)
    {
        // 1. Запрашиваем создание ToolView у главного окна Kate
        QWidget* tv = mainWindow->createToolView(
            plugin,
            QStringLiteral("test_panel"),
            KTextEditor::MainWindow::Left,
            QIcon::fromTheme(QStringLiteral("dialog-information")),
            QStringLiteral("TEST")
        );

        if (!tv) {
            qWarning() << "Ошибка: Не удалось создать ToolView!";
            return;
        }

        tv->setMinimumWidth(200);
        tv->setMinimumHeight(100);

        // 2. Создаем свой виджет-контейнер, чтобы не нарушать внутренний layout Kate
        QWidget* container = new QWidget(tv);
        QVBoxLayout* layout = new QVBoxLayout(container);
        layout->setContentsMargins(10, 10, 10, 10);

        // 3. Создаем и стилизуем текстовую метку
        QLabel* label = new QLabel(QStringLiteral("HELLO WORLD"), container);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(QStringLiteral("color: red; background: yellow; font-size: 20px;"));

        layout->addWidget(label);

        // 4. Безопасно встраиваем наш контейнер в существующий макет панели
        if (tv->layout()) {
            tv->layout()->addWidget(container);
        } else {
            // Резервный вариант, если у возвращенного виджета изначально нет layout
            QVBoxLayout* rootLayout = new QVBoxLayout(tv);
            rootLayout->setContentsMargins(0, 0, 0, 0);
            rootLayout->addWidget(container);
        }

        qDebug() << "ToolView успешно создан, текст метки:" << label->text();
    }
};

// Реализация фабричного метода для создания представления плагина
QObject* TestPlugin::createView(KTextEditor::MainWindow* mainWindow)
{
    return new TestPluginView(this, mainWindow);
}

// Экспорт фабрики плагина с указанием JSON-файла метаданных
K_PLUGIN_FACTORY_WITH_JSON(TestPluginFactory, "3decommand.json", registerPlugin<TestPlugin>();)

#include "plugin.moc"

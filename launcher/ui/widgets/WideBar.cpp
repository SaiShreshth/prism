#include "WideBar.h"
#include <QToolButton>

class ActionButton : public QToolButton {
    Q_OBJECT
   public:
    ActionButton(QAction* action, QWidget* parent = nullptr) : QToolButton(parent), m_action(action)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        connect(action, &QAction::changed, this, &ActionButton::actionChanged);
        connect(this, &ActionButton::clicked, action, &QAction::trigger);

        actionChanged();
    };
   private slots:
    void actionChanged()
    {
        setEnabled(m_action->isEnabled());
        setChecked(m_action->isChecked());
        setCheckable(m_action->isCheckable());
        setText(m_action->text());
        setIcon(m_action->icon());
        setToolTip(m_action->toolTip());
        setHidden(!m_action->isVisible());
        setFocusPolicy(Qt::NoFocus);
    }

   private:
    QAction* m_action;
};

WideBar::WideBar(const QString& title, QWidget* parent) : QToolBar(title, parent)
{
    setFloatable(false);
    setMovable(false);
}

WideBar::WideBar(QWidget* parent) : QToolBar(parent)
{
    setFloatable(false);
    setMovable(false);
}

void WideBar::addAction(QAction* action)
{
    BarEntry entry;
    entry.bar_action = addWidget(new ActionButton(action, this));
    entry.menu_action = action;
    entry.type = BarEntry::Type::Action;

    m_entries.push_back(entry);
}

void WideBar::addSeparator()
{
    BarEntry entry;
    entry.bar_action = QToolBar::addSeparator();
    entry.type = BarEntry::Type::Separator;

    m_entries.push_back(entry);
}

auto WideBar::getMatching(QAction* act) -> QList<BarEntry>::iterator
{
    auto iter = std::find_if(m_entries.begin(), m_entries.end(), [act](BarEntry const& entry) { return entry.menu_action == act; });

    return iter;
}

void WideBar::insertActionBefore(QAction* before, QAction* action)
{
    auto iter = getMatching(before);
    if (iter == m_entries.end())
        return;

    BarEntry entry;
    entry.bar_action = insertWidget(iter->bar_action, new ActionButton(action, this));
    entry.menu_action = action;
    entry.type = BarEntry::Type::Action;

    m_entries.insert(iter, entry);
}

void WideBar::insertActionAfter(QAction* after, QAction* action)
{
    auto iter = getMatching(after);
    if (iter == m_entries.end())
        return;

    BarEntry entry;
    entry.bar_action = insertWidget((iter + 1)->bar_action, new ActionButton(action, this));
    entry.menu_action = action;
    entry.type = BarEntry::Type::Action;

    m_entries.insert(iter + 1, entry);
}

void WideBar::insertSpacer(QAction* action)
{
    auto iter = getMatching(action);
    if (iter == m_entries.end())
        return;

    auto* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    BarEntry entry;
    entry.bar_action = insertWidget(iter->bar_action, spacer);
    entry.type = BarEntry::Type::Spacer;
    m_entries.insert(iter, entry);
}

void WideBar::insertSeparator(QAction* before)
{
    auto iter = getMatching(before);
    if (iter == m_entries.end())
        return;

    BarEntry entry;
    entry.bar_action = QToolBar::insertSeparator(before);
    entry.type = BarEntry::Type::Separator;

    m_entries.insert(iter, entry);
}

QMenu* WideBar::createContextMenu(QWidget* parent, const QString& title)
{
    auto* contextMenu = new QMenu(title, parent);
    for (auto& item : m_entries) {
        switch (item.type) {
            default:
            case BarEntry::Type::None:
                break;
            case BarEntry::Type::Separator:
            case BarEntry::Type::Spacer:
                contextMenu->addSeparator();
                break;
            case BarEntry::Type::Action:
                contextMenu->addAction(item.menu_action);
                break;
        }
    }
    return contextMenu;
}

#include "WideBar.moc"

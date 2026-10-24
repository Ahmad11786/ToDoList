
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QLineEdit>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
 resize(600, 700);
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #111827;
        }

        QLabel {
            color: #F9FAFB;
            font-family: "Segoe UI";
            font-size: 14px;
        }

        QLineEdit {
            background-color: #1F2937;
            color: #FFFFFF;
            border: 1px solid #374151;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
        }

        QLineEdit:focus {
            border: 1px solid #3B82F6;
        }

        QPushButton {
            background-color: #2563EB;
            color: #FFFFFF;
            border: none;
            border-radius: 8px;
            padding: 10px 16px;
            font-size: 14px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #1D4ED8;
        }

        QPushButton:pressed {
            background-color: #1E40AF;
        }

        QListWidget {
            background-color: #F9FAFB;
            color: #111827;
            border: none;
            border-radius: 10px;
            padding: 8px;
            font-size: 14px;
        }

        QListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #E5E7EB;
        }

        QListWidget::item:selected {
            background-color: #DBEAFE;
            color: #111827;
        }

QPushButton#addButton {
    min-width: 150px;
}
        QPushButton#deleteButton {
            background-color: #DC2626;
        }

        QPushButton#deleteButton:hover {
            background-color: #B91C1C;
        }
    )");



    connect(ui->addButton, &QPushButton::clicked,this, &MainWindow::addTask);

    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteTask);

    // Load saved tasks when the application starts.
    loadTasks();

    // Update task appearance, save changes, and update counters.
    connect(ui->taskList, &QListWidget::itemChanged,
            this, [this](QListWidgetItem *item)
            {
                bool checked = (item->checkState() == Qt::Checked);

                QFont font = item->font();

                if (font.strikeOut() != checked)
                {
                    font.setStrikeOut(checked);
                    item->setFont(font);
                }

                saveTasks();
                updateTaskCount();
            });

    updateTaskCount();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTask()
{
    QString task = ui->taskInput->text().trimmed();

    if (task.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Empty Task",
            "Please enter a task first!"
            );

        return;
    }

    QListWidgetItem *item = new QListWidgetItem(task);

    item->setFlags(
        item->flags() | Qt::ItemIsUserCheckable
        );

    item->setCheckState(Qt::Unchecked);

    ui->taskList->addItem(item);

    ui->taskInput->clear();

    saveTasks();
    updateTaskCount();
}

void MainWindow::deleteTask()
{
    bool taskDeleted = false;

    for (int i = ui->taskList->count() - 1; i >= 0; i--)
    {
        QListWidgetItem *item = ui->taskList->item(i);

        if (item->checkState() == Qt::Checked)
        {
            delete ui->taskList->takeItem(i);
            taskDeleted = true;
        }
    }

    if (taskDeleted)
    {
        saveTasks();
    }
    else
    {
        QMessageBox::warning(
            this,
            "No Task Checked",
            "Please check a task before deleting it!"
            );
    }

    updateTaskCount();
}

void MainWindow::updateTaskCount()
{
    int total = ui->taskList->count();
    int completed = 0;

    for (int i = 0; i < total; i++)
    {
        QListWidgetItem *item = ui->taskList->item(i);

        if (item->checkState() == Qt::Checked)
        {
            completed++;
        }
    }

    int remaining = total - completed;

    ui->totalTasksLabel->setText(
        "Total Tasks: " + QString::number(total)
        );

    ui->completedTasksLabel->setText(
        "Completed: " + QString::number(completed)
        );

    ui->remainingTasksLabel->setText(
        "Remaining: " + QString::number(remaining)
        );
}

void MainWindow::saveTasks()
{
    QString filePath =
        QCoreApplication::applicationDirPath() + "/tasks.txt";

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(
            this,
            "File Error",
            "Could not save tasks to the file."
            );

        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < ui->taskList->count(); i++)
    {
        QListWidgetItem *item = ui->taskList->item(i);

        out << (item->checkState() == Qt::Checked ? "1" : "0")
            << '\t'
            << item->text()
            << '\n';
    }

    file.close();
}

void MainWindow::loadTasks()
{
    QString filePath =
        QCoreApplication::applicationDirPath() + "/tasks.txt";

    QFile file(filePath);

    // It is normal for this file not to exist on the first run.
    if (!file.exists())
    {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(
            this,
            "File Error",
            "Could not load saved tasks."
            );

        return;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.length() < 2 || line[1] != '\t')
        {
            continue;
        }

        QString task = line.mid(2);

        QListWidgetItem *item = new QListWidgetItem(task);

        item->setFlags(
            item->flags() | Qt::ItemIsUserCheckable
            );

        if (line[0] == '1')
        {
            item->setCheckState(Qt::Checked);
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }

        QFont font = item->font();
        font.setStrikeOut(item->checkState() == Qt::Checked);
        item->setFont(font);

        ui->taskList->addItem(item);
    }

    file.close();
}





void MainWindow::on_pushButton_clicked()
{
    // Get the selected task
    QListWidgetItem *item = ui->taskList->currentItem();

    // Check whether a task is selected
    if (item == nullptr)
    {
        QMessageBox::warning(
            this,
            "No Task Selected",
            "Please select a task to edit!"
            );

        return;
    }

    // Store the old task text
    QString oldTask = item->text();

    bool ok = false;

    // Ask the user for the updated task
    QString newTask = QInputDialog::getText(
        this,
        "Edit Task",
        "Enter the updated task:",
        QLineEdit::Normal,
        oldTask,
        &ok
        );

    // Stop if the user clicks Cancel
    if (!ok)
    {
        return;
    }

    // Remove extra spaces
    newTask = newTask.trimmed();

    // Validate the new task
    if (newTask.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Invalid Task",
            "Task cannot be empty!"
            );

        return;
    }

    // Update the task text
    item->setText(newTask);

    // Preserve the completed-task formatting
    QFont font = item->font();
    font.setStrikeOut(item->checkState() == Qt::Checked);
    item->setFont(font);

    // Save changes and update counters
    saveTasks();
    updateTaskCount();
}



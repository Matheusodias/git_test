#include "../../Interface/inc/table.h"
#include <QHeaderView>
#include <QDebug>
Table::Table(Experiment *parent, QTableWidget *densification, QTableWidget *shear)
{
    table_variables = parent;

    densificationTable = densification;
    shearTable = shear; 
    

    // updateData_StaticTable(QTableWidget *table_widget,option)
    // initialConfig_StaticTable(QTableWidget *table_widget, uint8_t option)
    // updateData_DinamycTable(QTableWidget *table_widget, uint8_ option)
    // initialConfig_DinamicTable(QTableWidget *table_widget,uint8_ option)
    
    QString densificationHeaderNames[] = {
       "N. amostra","Dia", "H:min:seg:ms","Desl.ver.(mm)",
        "Carga nor.(KPa)",
    };

    QString shearHeaderNames []= {
        "N. amostra", "Dia", "H:m:s",
        "Des.h.(mm)","Car.cis.(KPa)",
        "Tensão nor.","Tensão cis."
    };



    QString phasesLinesNames []= {
        "Área (cm²)", "Volume inicial (cm³)","Massa específica úmida inicial da amostra de solo (g/cm³)",
        "Massa específica seca inicial da amostra de solo (g/cm³)","Índice de vazios","Peso Específico da água",
        "Saturação inicial da amostra de solo (%)"
    };

    QString infoLinesNames []= {
        "Nome do experimento", "Nome do operador", "Tempo de início do experimento (s)",  "Tempo atual (s)",
        "Tipo de teste", "Tipo de espécime",
        "Classe USCS", "Classe ASHTO", "Preparações da amostra", "Id da amostra",
        "Número do furo", "Localização da amostra", "Descrição da amostra",
        "Altura inicial (cm)", "Peso úmido inicial (g)", "Umidade inicial (%)",
        "Peso específico dos sólidos (g/cm³)", "Limite de plasticidade (%)",
        "Limite de liquidez (%)", "Posição inicial (cm)", "Diamêtro (cm)",
        "Pressão", "Área (cm²)", "Volume inicial (cm³)","Massa específica úmida inicial da amostra de solo (g/cm³)",
        "Massa específica seca inicial da amostra de solo (g/cm³)","Índice de vazios","Peso Específico da água",
        "Saturação inicial da amostra de solo (%)"
    };

    QString densificationResultLinesName[] = {
        "Tempo Total Adensamento",
        "Leitura da carga final (KPa)",
        "Altura final (mm)",
        "Mudança de altura (mm)",
        "Volume Final adensamento (cm³)"
    };

    QString shearResultLinesName[] = {
        "Tempo total Cisalhamento",
        "Carga de cisalhamento final (KPa)",
        "Carga normal final (KPa)",
        "Tensão de cisalhamento máxima (KPa)",
        "Tensão normal máxima (KPa)",
        "Deslocamento vertical total (cm)",
        "Deslocamento horizontal total (cm)",
        "Velocidade média(mm/min)",
        "Mudança de altura(mm)",
        "Volume final (cm³)"
    };

   


    
    for(long unsigned int i=0;i<sizeof(phasesLinesNames)/sizeof(QString);i++){
        this->lineNames[phases_table].push_back(phasesLinesNames[i]);
    }

    for(long unsigned int i=0;i<sizeof(infoLinesNames)/sizeof(QString);i++){
        this->lineNames[info_table].push_back(infoLinesNames[i]);
    }

    for(long unsigned int i=0;i<sizeof(densificationHeaderNames)/sizeof(QString);i++){
        this->headerNames[design_densification_table].push_back(densificationHeaderNames[i]);
    }

    for(long unsigned int i=0;i<sizeof(shearHeaderNames)/sizeof(QString);i++){
        this->headerNames[design_shear_table].push_back(shearHeaderNames[i]);
    }

    for(long unsigned int i=0;i<sizeof(densificationResultLinesName)/sizeof(QString);i++){
        this->lineNames[densification_result_table].push_back(densificationResultLinesName[i]);
    }

    for(long unsigned int i=0;i<sizeof(shearResultLinesName)/sizeof(QString);i++){
        this->lineNames[shear_result_table].push_back(shearResultLinesName[i]);
    }


}

void Table::customizeTable(QTableWidget *table_widget)
{

    table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    table_widget->setStyleSheet("QTableWidget{"
                                "background-color: white;"
                                "alternate-background-color: #77A0AC;"
                                "selection-background-color: #0D495C;"
                                "font:  15pt 'Ubuntu'; border:0px; }"
                                "QHeaderView { font: bold 20pt 'Ubuntu'; }"
                                );

    table_widget->setAlternatingRowColors(true);

    // cell item properties
    table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_widget->setTextElideMode(Qt::ElideRight);

    table_widget->setShowGrid(true);
    table_widget->setGridStyle(Qt::DotLine);

    table_widget->horizontalHeader()->setVisible(true);
    table_widget->verticalHeader()->setVisible(false);
    //table_widget->horizontalHeader()->setDefaultSectionSize(150);
    //table_widget->horizontalHeader()->setStretchLastSection(true);


    //table_widget->horizontalHeader()->setFont(QFont ( "Ubuntu", 50, QFont::Normal));
    table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch) ;
    table_widget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

}

void Table::initialConfig_StaticTable(QTableWidget *table_widget, uint8_t option)
{
    table_widget->setColumnCount(2);
    QStringList headerLabels;
    headerLabels << "Variável" << "Valor";
    table_widget->setHorizontalHeaderLabels(headerLabels);

    int row_count =  this->lineNames[option].size();
    table_widget->setRowCount(row_count);

    for(int i=0;i<row_count;i++){
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(this->lineNames[option][i]);
        //item->setTextAlignment(Qt::AlignHCenter);
        item->setTextAlignment(Qt::AlignCenter);
        table_widget->setItem(i,0,item);

    }
}

void Table::updateData_StaticTable(QTableWidget *table_widget,uint8_t option)
{
    if(option == phases_table){
        columnValues = table_variables->getAllData_forPhasesTable();
    } else if(option == info_table) {
        columnValues = table_variables->getAllData_forInfoTable();
    } else if(option == densification_result_table) {
        columnValues = table_variables->updateDensificationResultsTable();
    } else if(option == shear_result_table) {
        columnValues = table_variables->updateShearResultsTable();
    }

    for(int i=0;i<columnValues.size();i++){
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(columnValues[i]);
        item->setTextAlignment(Qt::AlignCenter);
        table_widget->setItem(i,1,item);
    }
}

void Table::initialConfig_DynamicTable(QTableWidget *table_widget,uint8_t option)
{
    if(option == design_shear_table){
        table_widget->setStyleSheet(
            "QTableWidget{"
            "background-color: white;"
            "alternate-background-color: #77A0AC;"
            "selection-background-color: #0D495C;"
            "font:  15pt 'Ubuntu'; border:0px; }"
            "QHeaderView { font: bold 13pt 'Ubuntu'; }"
        );
    } else{
        table_widget->setStyleSheet(
            "QTableWidget{"
            "background-color: white;"
            "alternate-background-color: #77A0AC;"
            "selection-background-color: #0D495C;"
            "font:  20pt 'Ubuntu'; border:0px; }"
            "QHeaderView { font: bold 15pt 'Ubuntu'; }"
        );
    }
    
    table_widget->setColumnCount(this->headerNames[option].size());
    table_widget->setHorizontalHeaderLabels(headerNames[option]);
}

void Table::updateData_DynamicTable(QStringList data, uint8_t option)
{ 


     QTableWidget *table_widget;
     machineTablevalues[option] = data;

     if(option == design_densification_table){
        table_widget = densificationTable;
     } else {
        table_widget = shearTable;
     }

     table_widget->insertRow( table_widget->rowCount());
     for(int j=0;j<machineTablevalues[option].size();j++){
         QTableWidgetItem *item = new QTableWidgetItem;
         item->setText(machineTablevalues[option][j]);
         item->setTextAlignment(Qt::AlignCenter);
         table_widget->setItem (table_widget->rowCount()-1,j,item);
     }


}


void Table::initialConfig_ShearTable(QTableWidget * table_widget)
{
    //table_widget->setRowCount(1);
    table_widget->setStyleSheet("QTableWidget{"
                                "background-color: white;"
                                "alternate-background-color: #77A0AC;"
                                "selection-background-color: #0D495C;"
                                "font:  20pt 'Ubuntu'; border:0px; }"
                                "QHeaderView { font: bold 15pt 'Ubuntu'; }"
                                );


    QString lineNames []= {
        "Número da amostra", "Dia", "H:min:seg:ms",
        "Desl. ver. (mm)", "Desl. hori. (mm)",
        "Carga cis. (KPa)", "Carga normal (KPa)", "Tensão normal",
        "Tensão cisalhante"
    };
    int tamanho = sizeof(lineNames)/sizeof(QString);
    table_widget->setColumnCount(tamanho);
    QStringList headerLabels;
    for(int i=0;i<tamanho;i++){
        headerLabels.push_back(lineNames[i]);
    }
    table_widget->setHorizontalHeaderLabels(headerLabels);

}

void Table::updateData_ShearTable(QTableWidget *table_widget)
{
    //QStringList data = table_variables->getAllData();
    
   
    table_widget->insertRow( table_widget->rowCount());
    for(int j=0;j<9;j++){
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString::number(j));
        item->setTextAlignment(Qt::AlignCenter);
        table_widget->setItem (table_widget->rowCount()-1,j,item);    
    }
    
    
    
    
}

void Table::clearDynamicTables(QTableWidget *table_widget)
{
    table_widget->setRowCount(0);
}

void Table::clearStaticTables(QTableWidget *table_widget)
{
    int size = table_widget->rowCount();
    for(int i=0;i<size;i++){
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText("");
        item->setTextAlignment(Qt::AlignCenter);
        table_widget->setItem(i,1,item);
    }
}

void Table::test()
{

    columnValues = table_variables->getAllData_forPhasesTable();


    for(int i=0;i<columnValues.size();i++){
        qDebug() << columnValues[i];
    }
}



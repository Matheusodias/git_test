#include "../../Interface/inc/button.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QToolButton>
/**
 * @brief Constrói um novo objeto da classe Button.
 * 
 * Esta classe tem como objetivo criar as variáveis de estilo, imagem, ícones e outras
 * configurações iniciais dos botões;
 * 
 */
Button::Button()
{

    // Add styleSheets
    this->styleSheetConfiguration();

    // Add images
    this->imageConfiguration();

    // Add pixmaps
    this->iconsConfiguration();

    // Adjust sizes
    this->buttonSizeConfiguration();

    this->clickedButton[0] = nullptr;
    this->clickedButton[1] = nullptr;
    this->clickedButton[2] = nullptr;
}
/**
 * @brief Cria as folhas de estilo para os botões.
 * 
 */
void Button::styleSheetConfiguration()
{
    QString headerButton_lightStyleSheet; 
    QString headerButton_darkStyleSheet; 
    QString phasesButton_lightStyleSheet; 
    QString phasesButton_darkStyleSheet; 
    QString continueButton_StyleSheet; 
    QString green_pressureButton_StyleSheet;
    QString red_pressureButton_StyleSheet;
    QString disabled_positionButton_StyleSheet;
    QString cancelButton_StyleSheet;
    QString initShear_button_StyleSheet;
    QString velocityPosition_button_StyleSheet;

    phasesButton_lightStyleSheet = "QToolButton{ background-color:white; border-bottom:3px solid #0D495C;"
                                         "border-right:3px solid #0D495C;border-top:2px solid #0D495C;"
                                         "border-left:2px solid #0D495C;border-radius:10px; color:#10576D;font: 700 12pt 'Ubuntu';}"
                                         "QToolButton:pressed {background-color:rgb(229, 229, 229);}";

    phasesButton_darkStyleSheet = "QToolButton{ background-color:#77A0AC; border-bottom:3px solid #0D495C;"
                                         "border-right:3px solid #0D495C;border-top:2px solid #0D495C;"
                                         "border-left:2px solid #0D495C;border-radius:10px; color:white;font: 700 12pt 'Ubuntu';}"
                                         "QToolButton:pressed {background-color:#0D495C;}";


    continueButton_StyleSheet = "QToolButton{ background-color:#0E3A47; border-bottom:2px solid rgb(7, 36, 44);"
                                         "border-right:1px solid rgb(7, 36, 44);border-top:1px solid rgb(7, 36, 44);"
                                         "border-left:1px solid rgb(7, 36, 44);border-radius:10px;font: 700 13pt 'Ubuntu';"
                                           "color: white; padding:3;}"
                                          "QToolButton:pressed {background-color:black;}";



    headerButton_lightStyleSheet = "QToolButton{ background-color:white; border-bottom:5px solid rgb(227, 227, 227);"
                                          "border-right:5px solid rgb(227, 227, 227); border-top:1px solid rgb(227, 227, 227);"
                                          "border-left:1px solid rgb(227, 227, 227); border-radius:25px;"
                                          "font: 700 10pt 'Ubuntu'; color: #10576D;}"
                                          "QToolButton:pressed {background-color:rgb(229, 229, 229);}";

    headerButton_darkStyleSheet = "QToolButton{ background-color:#0D495C; border-bottom:5px solid rgb(39, 64, 71);"
                                         "border-right:5px solid rgb(39, 64, 71); border-top:1px solid rgb(39, 64, 71);"
                                         "border-left:1px solid rgb(39, 64, 71); border-radius:25px;"
                                         "font: 700 10pt 'Ubuntu'; color: white;}"
                                         "QToolButton:pressed {background-color:#072a36;}";


    red_pressureButton_StyleSheet = "QToolButton{ background-color: #FF0000; border-bottom:2px solid #A00000;"
                                     "border-right:1px solid #A00000;border-top:1px solid #A00000;"
                                     "border-left:1px solid #A00000;border-radius:60px;font: 700 20pt 'Ubuntu';"
                                       "color: white; padding:3;}"
                                      "QToolButton:pressed {background-color:#A00000;}";

    green_pressureButton_StyleSheet = "QToolButton{ background-color:#3F953F; border-bottom:2px solid #295D29;"
                                     "border-right:1px solid #295D29;border-top:1px solid #295D29;"
                                     "border-left:1px solid #295D29;border-radius:60px;font: 700 20pt 'Ubuntu';"
                                       "color: white; padding:3;}"
                                      "QToolButton:pressed {background-color:#295D29;}";

    disabled_positionButton_StyleSheet = "QToolButton{ background-color:#989898; border-bottom:2px solid #989898;"
                                         "border-right:1px solid #989898;border-top:1px solid #989898;"
                                         "border-left:1px solid #989898;border-radius:10px;font: 700 13pt 'Ubuntu';"
                                           "color: white; padding:3;}"
                                          "QToolButton:pressed {background-color:#989898;}";


    cancelButton_StyleSheet = "QToolButton{ background-color:rgb(171, 26, 36); border-bottom:2px solid #87161D;"
                                     "border-right:1px solid #87161D;border-top:1px solid #87161D;"
                                     "border-left:1px solid #87161D;border-radius:10px;font: 700 13pt 'Ubuntu';"
                                       "color: white; padding:3;}"
                                      "QToolButton:pressed {background-color:#87161D;}";

    initShear_button_StyleSheet = "QToolButton{ background-color:rgb(38, 162, 105); border-bottom:2px solid #1B7A4F;"
                                  "border-right:1px solid #1B7A4F;border-top:1px solid #1B7A4F;"
                                  "border-left:1px solid #1B7A4F;border-radius:10px;font: 700 13pt 'Ubuntu';"
                                    "color: white; padding:3;}"
                                   "QToolButton:pressed {background-color:#1B7A4F;}";



    velocityPosition_button_StyleSheet = "QToolButton{ background-color:white; border-bottom:3px solid #0D495C;"
                                  "border-right:2px solid #0D495C;border-top:2px solid #0D495C;"
                                  "border-left:2px solid #0D495C;border-radius:15px;font: 700 13pt 'Ubuntu';"
                                    "color: black; padding:3;}"
                                   "QToolButton:pressed {background-color:#0D495C; color: white;}";

//#3F953F
//#295D29
//#FF0000
//#A00000



    this->button_styleSheets[headerButton_lightBackgroundColor] = headerButton_lightStyleSheet;
    this->button_styleSheets[headerButton_darkBackgroundColor]  = headerButton_darkStyleSheet;
    this->button_styleSheets[phasesButton_lightBackgroundColor] = phasesButton_lightStyleSheet;
    this->button_styleSheets[phasesButton_darkBackgroundColor]  = phasesButton_darkStyleSheet;
    this->button_styleSheets[continueButton_BackgroundColor] = continueButton_StyleSheet;
    this->button_styleSheets[pressureButton_GreenBackgroundColor] = green_pressureButton_StyleSheet;
    this->button_styleSheets[pressureButton_RedBackgroundColor] = red_pressureButton_StyleSheet;
    this->button_styleSheets[moveButton_DisabledBackgroundColor] = disabled_positionButton_StyleSheet;

    this->button_styleSheets[cancelButton_BackgroundColor] = cancelButton_StyleSheet;
    this->button_styleSheets[initShearButton_BackgroundColor] = initShear_button_StyleSheet;
    this->button_styleSheets[velocityPositionBackgroundColor] = velocityPosition_button_StyleSheet;

    

}
/**
 * @brief Insere no array as imagens utilizadas na aplicação.
 * 
 */
void Button::imageConfiguration()
{
    this->button_images[historyButton_lightIcon] = ":light_history.png";
    this->button_images[historyButton_darkIcon] = ":dark_history.png";
    this->button_images[experimentButton_lightIcon] =  ":light_experiment.png";
    this->button_images[experimentButton_darkIcon] = ":dark_experiment.png";
    this->button_images[configurationButton_lightIcon] = ":light_configuration.png";
    this->button_images[configurationButton_darkIcon] = ":dark_configuration.png";
    this->button_images[continueButton_Icon] = ":Arrow.png";
    this->button_images[densificationButton_lightIcon] = ":light_densification.png";
    this->button_images[densificationButton_darkIcon] = ":dark_densification.png";
    this->button_images[shearButton_lightIcon] =  ":light_shear.png";
    this->button_images[shearButton_darkIcon] = ":dark_shear.png";
    this->button_images[infoButton_lightIcon] = ":light_info.png";
    this->button_images[infoButton_darkIcon] = ":dark_info.png";

    this->button_images[finishButton_Icon] = ":finish.png";

}

/**
 * @brief Adiciona as imagens aos ícones para facilitar a estilização.
 * 
 */
void Button::iconsConfiguration()
{


    this->button_icons[historyButton_lightIcon].addPixmap(QPixmap(this->button_images[historyButton_lightIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[historyButton_darkIcon].addPixmap(QPixmap(this->button_images[historyButton_darkIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[experimentButton_lightIcon].addPixmap(QPixmap(this->button_images[experimentButton_lightIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[experimentButton_darkIcon].addPixmap(QPixmap(this->button_images[experimentButton_darkIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[configurationButton_lightIcon].addPixmap(QPixmap(this->button_images[configurationButton_lightIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[configurationButton_darkIcon].addPixmap(QPixmap(this->button_images[configurationButton_darkIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[continueButton_Icon].addPixmap(QPixmap(this->button_images[continueButton_Icon]),QIcon::Normal,QIcon::On);

    this->button_icons[densificationButton_lightIcon].addPixmap(QPixmap(this->button_images[densificationButton_lightIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[densificationButton_darkIcon].addPixmap(QPixmap(this->button_images[densificationButton_darkIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[shearButton_lightIcon].addPixmap(QPixmap(this->button_images[shearButton_lightIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[shearButton_darkIcon].addPixmap(QPixmap(this->button_images[shearButton_darkIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[infoButton_lightIcon].addPixmap(QPixmap(this->button_images[infoButton_lightIcon]),QIcon::Normal,QIcon::On);
    this->button_icons[infoButton_darkIcon].addPixmap(QPixmap(this->button_images[infoButton_darkIcon]),QIcon::Normal,QIcon::On);

    this->button_icons[finishButton_Icon].addPixmap(QPixmap(this->button_images[finishButton_Icon]),QIcon::Normal,QIcon::On);
}


/**
 * @brief Insere os valores mínimos e máximos do tamanho dos botões.
 * 
 */
void Button::buttonSizeConfiguration()
{
    this->buttonMaximumSize[outsideExperiment_buttonSize] = QSize(300, 80);
    this->buttonMinimumSize[outsideExperiment_buttonSize] = QSize(130, 50);

    this->buttonMaximumSize[phases_buttonSize] = QSize(16777215, 16777215);
    this->buttonMinimumSize[phases_buttonSize] = QSize(80, 25);


    this->buttonMaximumSize[phases_continueButtonSize] = QSize(200, 100);
    this->buttonMinimumSize[phases_continueButtonSize] = QSize(140, 30);


    this->buttonMaximumSize[cancelButton_size] = QSize(300, 80);
    this->buttonMinimumSize[cancelButton_size] = QSize(130, 30);

    this->buttonMaximumSize[initShearButton_size] = QSize(300, 80);
    this->buttonMinimumSize[initShearButton_size] = QSize(220, 30);


    this->buttonMaximumSize[velocityPositionButton_size] = QSize(300, 80);
    this->buttonMinimumSize[velocityPositionButton_size] = QSize(30, 10);


    this->buttonMaximumSize[exportButton_size] = QSize(200, 100);
    this->buttonMinimumSize[exportButton_size] = QSize(160, 30);


}

/**
 * @brief Responsável por alterar a página e o estilo dos botões do cabeçalho ao trocar de página.
 * 
 * @param insideExperiment Utilizado para trocar de StackWidget na troca de páginas.
 */
void Button::changeHeaderPage_InsideExperiment(QStackedWidget *insideExperiment)
{

    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender());
    QString object_name = buttonSender->objectName();
    int next_page = 0;

    QString buttons_name[] = {"densification_button","shear_button","info_button"};
    uint8_t array_size = sizeof(buttons_name)/sizeof(QString);


    for(int i=0;i<array_size;i++){
        if(object_name == buttons_name[i]){
            next_page = i;
        }
    }

    uint8_t icon[3] = {densificationButton_lightIcon,shearButton_lightIcon,infoButton_lightIcon};
    uint8_t choosen_icon = icon[next_page];
    insideExperiment->setCurrentIndex(next_page);
    this->changeButton_style(buttonSender, choosen_icon, headerButton_lightBackgroundColor,0);

}

/**
 * @brief Responsável por alterar entre as páginas de gráficos/tabelas/resultados dentro da página Adensamento ou Cisalhamento.
 * 
 * @param insideExperiment Utilizado para trocar de StackWidget na troca de páginas.
 * @param isDensificationPage Utilizado para identificar se é a página de Adensamento ou Cisalhamento
 */
void Button::changePage_InsideExperiment(QStackedWidget *insideExperiment, bool isDensificationPage)
{
    QToolButton* buttonSender = qobject_cast<QToolButton*>(sender()); // retrieve the button you have clicked
    QString object_name = buttonSender->objectName();
    int next_page = 0;

    QString * buttons_name;
    QString densification_buttons[] = {"densificationGraphs_toolButton", "densificationTable_toolButton", "densificationResult_toolButton"};
    QString shear_buttons[] = {"shearGraphs_toolButton", "shearTable_toolButton", "shearResult_toolButton"};
    buttons_name = isDensificationPage?densification_buttons:shear_buttons;
    int pos = isDensificationPage? 1 : 2;


    uint8_t array_size = 3;


    for(int i=0;i<array_size;i++){
        if(object_name == buttons_name[i]){
            next_page = i;
        }
    }

    insideExperiment->setCurrentIndex(next_page);
    this->changeButton_style(buttonSender, no_icon, phasesButton_lightBackgroundColor,pos);

}



/**
 * @brief Estiliza um QPushButton ou QToolButton para ter sombra.
 * 
 * @param button O botão que será estilizado.
 */
void Button::setButtonShadow(QToolButton * button)
{

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5); //Adjust accordingly
    effect->setOffset(5,5); //Adjust accordingly
    button->setGraphicsEffect(effect);
    return;
}

/**
 * @brief Altera o ícone e estilo do botão.
 * 
 * @param button Botão que será alterado.
 * @param style Estilo do botão, podendo ser claro ou escuro. 
 * @param icon  Ícone do botão.
 */
void Button::setButton_style_icon(QToolButton * button, uint8_t style, uint8_t icon)
{
    if(icon != no_icon){
        button->setIcon(button_icons[icon]);
    }
    button->setStyleSheet(this->button_styleSheets[style]);
    return;
}

/**
 * @brief Estilização inicial do botão, colocando-se sombra, estilo e tamanho.
 * 
 * @param button O botão que será alterado.
 * @param style_option  A folha de estilo.
 * @param size O tamanho do botão.
 */
void Button::initialButtonStyling(QToolButton * button, uint8_t style_option, uint8_t size)
{
    this->setButtonShadow(button);
    button->setStyleSheet(this->button_styleSheets[style_option]);
    button->setMaximumSize(this->buttonMaximumSize[size]);
    button->setMinimumSize(this->buttonMinimumSize[size]);
}
/**
 * @brief Altera a aparência do botão iniciar cisalhamento para finalizar o experimento.
 * 
 * @param button Botão que será alterado.
 */
void Button::changeInitShear_toFinishButton(QToolButton *button)
{
    if(button->isChecked()){
        this->initialButtonStyling(button, continueButton_BackgroundColor , phases_buttonSize);
        button->setText("Finalizar");
        button->setIcon(button_icons[finishButton_Icon]);
    } else {
        this->initialButtonStyling(button, initShearButton_BackgroundColor, initShearButton_size);
        button->setText("Iniciar cisalhamento");
        button->setIcon(button_icons[continueButton_Icon]);
    }
}


/**
 * @brief Realiza a estilização de botões dentro de 1 layout.
 * 
 * Essa estilização é realizada iterando sobre os elementos do layout.
 * 
 * @param boxlayout Layout escolhido
 * @param style_option Estilo dos botões escolhido
 * @param size Tamanho dos botões escolhido
 */
void Button::initialButtonStyling_Layout(QHBoxLayout * boxlayout, uint8_t style_option, uint8_t size)
{
    QLayout *layout = boxlayout->layout();
    if (layout) {
        for (int i = 0; i < layout->count(); ++i){
            QToolButton * button = qobject_cast<QToolButton*>(layout->itemAt(i)->widget());
            if(button){

                this->setButtonShadow(button);
                button->setStyleSheet(this->button_styleSheets[style_option]);
                button->setMaximumSize(this->buttonMaximumSize[size]);
                button->setMinimumSize(this->buttonMinimumSize[size]);
                button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            }
       }
    }

    return;
}

/**
 * @brief 
 * 
 * 
 * @param current_button Botão que foi clicado
 * @param icon Ícone do botão clicado
 */

/**
 * @brief Altera a estilização do botão ao ser clicado.
 * 
 * Os botões de página mudam de cor ao serem clicados
 * 
 * @param current_button Botão escolhido
 * @param icon  Ícone do botão
 * @param style Estilo do botão
 * @param pos Identificação do cabeçalho de páginas atual 
 */
void Button::changeButton_style(QToolButton *current_button, uint8_t icon, uint8_t style, uint8_t pos)
{

    if(this->clickedButton[pos]==current_button){
        return;
    }
    else if(this->clickedButton[pos]!=nullptr && this->clickedButton[pos]!=current_button){
        if(icon==no_icon) {
            this->clickedButton[pos]->setStyleSheet(this->button_styleSheets[style]);
        } else {
            this->setButton_style_icon(this->clickedButton[pos], style, this->clickedButtonIcon[pos]);
        }

    }
    this->clickedButton[pos] = current_button;
    this->clickedButtonIcon[pos] = icon ;

    // Always sending the lightIcon, so to change to the darkIcon
    // you just need to add 1.
    // Same principle for background color, add 1 to button_lightBackgroundColor
    // to turn it dark.

    if(icon==no_icon){
        this->setButton_style_icon(current_button, ++style, icon);
    } else {
        this->setButton_style_icon(current_button, ++style, ++icon);
    }
    return;
}
/**
 * @brief Estilização dos botões de um Widget escolhido.
 * 
 * @param selectedWidget Widget escolhido.
 * @param style_option Estilo do botão escolhido.
 * @param size Tamanho do botão escolhido.
 */
void Button::initialButtonStyling_Widget(QObject *selectedWidget, uint8_t style_option, uint8_t size)
{
    QList<QToolButton*> selectedButtons = selectedWidget->findChildren<QToolButton*>();
    for(QList<QToolButton *>::iterator buttons = selectedButtons.begin();buttons != selectedButtons.end(); buttons++){
        this->setButtonShadow((*buttons));
        (*buttons)->setStyleSheet(this->button_styleSheets[style_option]);
        (*buttons)->setMaximumSize(this->buttonMaximumSize[size]);
        (*buttons)->setMinimumSize(this->buttonMinimumSize[size]);
        (*buttons)->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    }

    return;
}
/**
 * @brief Estilização do botão de início do experimento.
 * 
 * @param play_button Botão escolhido.
 * @param enabled Se verdadeiro, o botão está habilitado, se falso, está desabilidado.
 */
void Button::initExperiment_ButtonStyle(QToolButton *play_button, bool enabled)
{
    QString style;
    if(enabled){
        style =     "QToolButton{ background-color:#1E7C16; border-bottom:2px solid rgb(21, 82, 16);"
                    "border-right:1px solid rgb(21, 82, 16);border-top:1px solid rgb(21, 82, 16);"
                    "border-left:1px solid rgb(21, 82, 16);border-radius:10px;font: 700 20pt 'Ubuntu';"
                    "color: white; padding:3;}"
                    "QToolButton:pressed {background-color:rgb(21, 82, 16);}" ;
    } else{
        style = "QToolButton{ background-color:#989898; border-bottom:2px solid rgb(21, 82, 16);"
                "border-right:1px solid rgb(21, 82, 16);border-top:1px solid rgb(21, 82, 16);"
                "border-left:1px solid rgb(21, 82, 16);border-radius:10px;font: 700 20pt 'Ubuntu';"
                "color: white; padding:3;}"
                "QToolButton:pressed {background-color:#989898;}";
    }

    play_button->setStyleSheet(style);
}

/**
 * @brief Estilização do botão da pressão.
 * 
 * @param pressure Botão escolhido.
 */
void Button::pressureButton_style(QToolButton *pressure)
{
    this->setButton_style_icon(pressure, pressureButton_GreenBackgroundColor, no_icon);
    pressure->setMaximumSize( QSize(500, 200));


}









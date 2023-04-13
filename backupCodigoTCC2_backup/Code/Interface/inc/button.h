#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QStackedWidget>

#define headerButton_lightBackgroundColor 0 /*!< Número identificador do estilo do botão header com cores claras. */
#define headerButton_darkBackgroundColor 1 /*!< Número identificador do estilo do botão header com cores escuras. */
#define phasesButton_lightBackgroundColor 2 /*!< Número identificador do estilo do botão etapas com cores claras. */
#define phasesButton_darkBackgroundColor 3 /*!< Número identificador do estilo do botão etapas com cores escuras. */
#define continueButton_BackgroundColor 4 /*!< Número identificador do estilo do botão de continuar nas etapas. */
#define pressureButton_GreenBackgroundColor 5 /*!< Número identificador do estilo do botão de pressão ligado. */
#define pressureButton_RedBackgroundColor 6 /*!< Número identificador do estilo do botão de pressão desligado. */
#define moveButton_DisabledBackgroundColor 7 /*!< Número identificador do estilo do botão de mover desativado. */
#define cancelButton_BackgroundColor 8 /*!< Número identificador do estilo do botão de cancelar o experimento.*/
#define initShearButton_BackgroundColor 9 /*!< Número identificador do estilo do botão de iniciar o cisalhamento.*/
#define velocityPositionBackgroundColor 10  /*!< Número identificador do estilo dos botões de alterar a posição e velocidade.*/

#define historyButton_lightIcon 0 /*!< Número identificador do ícone do botão histórico para ser utilizado com o background claro.*/
#define historyButton_darkIcon 1 /*!< Número identificador do ícone do botão histórico para ser utilizado com o background escuro.*/
#define experimentButton_lightIcon 2 /*!< Número identificador do ícone do botão experimento para ser utilizado com o background claro.*/
#define experimentButton_darkIcon 3 /*!< Número identificador do ícone do botão experimento para ser utilizado com o background escuro.*/
#define configurationButton_lightIcon 4 /*!< Número identificador do ícone do botão configurações para ser utilizado com o background claro.*/
#define configurationButton_darkIcon 5 /*!< Número identificador do ícone do botão configurações para ser utilizado com o background escuro.*/
#define continueButton_Icon 6  /*!< Número identificador do ícone do botão continuar*/

#define densificationButton_lightIcon 7 /*!< Número identificador do ícone do botão adensamento para ser utilizado com o background claro.*/
#define densificationButton_darkIcon 8 /*!< Número identificador do ícone do botão adensamento para ser utilizado com o background escuro.*/
#define shearButton_lightIcon 9 /*!< Número identificador do ícone do botão cisalhamento para ser utilizado com o background claro.*/
#define shearButton_darkIcon 10 /*!< Número identificador do ícone do botão cisalhamento para ser utilizado com o background escuro.*/
#define infoButton_lightIcon 11 /*!< Número identificador do ícone do botão informações para ser utilizado com o background claro.*/
#define infoButton_darkIcon 12 /*!< Número identificador do ícone do botão informações para ser utilizado com o background escuro.*/
#define finishButton_Icon 13 /*!< Número identificador do ícone do botão finalizar.*/

#define no_icon 255 /*!< Número que informa que o botão não possui ícone. */
#define outsideExperiment_buttonSize 0   /*!< Número identificador do tamanho dos botões no cabeçalho . */
#define phases_buttonSize 1 /*!< Número identificador do tamanho dos botões das fases (etapa1,etapa2 ...). */
#define phases_continueButtonSize 2 /*!< Número identificador do tamanho dos botões de continuar . */
#define cancelButton_size 3 /*!< Número identificador do tamanho do botão de cancelar o experimento . */
#define initShearButton_size 4 /*!< Número identificador do tamanho do botão de iniciar o cisalhamento . */
#define velocityPositionButton_size 5 /*!< Número identificador do tamanho dos botões de ajustar as variáveis de velocidade e distância . */
#define exportButton_size 6 /*!< Número identificador do tamanho do botão de exportar tabelas.*/
/**
 * @brief Classe dos botões.
 * 
 * Esta classe é responsável por cuidar do comportamento e estilização dos botões.
 */
class Button: public QObject
{
    Q_OBJECT
public:
    Button();
    void setButtonShadow(QToolButton * button);
    void setButton_style_icon(QToolButton * button, uint8_t style, uint8_t icon);
    void initialButtonStyling_Layout(QHBoxLayout * boxlayout, uint8_t style_option, uint8_t size);
    void changeButton_style(QToolButton *current_button, uint8_t icon, uint8_t style, uint8_t pos);
    void initialButtonStyling_Widget(QObject * selectedWidget, uint8_t style_option, uint8_t size);
    void initExperiment_ButtonStyle(QToolButton *play_button, bool enabled);
    void pressureButton_style(QToolButton * pressure);
    void initialButtonStyling(QToolButton * button, uint8_t style_option, uint8_t size);

    void changeInitShear_toFinishButton(QToolButton * button);

private:
    QToolButton * clickedButton[3]; /*!< Botão do cabeçalho ou/e das etapas, que foi clicado. */
    uint8_t clickedButtonIcon[3]; /*!< Ícone do botão do cabeçalho ou/e de alguma subpágina que foi clicado. */

    QString button_styleSheets[15]; /*!< Array com os estilos claro e escudo.*/
    QString button_images[20]; /*!< Array com as imagens dos botões. */
    QIcon button_icons[20]; /*!< Array com os ícones dos botões. */


    QSize buttonMaximumSize[10];  /*!< Array com os tamanhos máximos dos botões. */
    QSize buttonMinimumSize[10]; /*!< Array com os tamanho mínimos dos botões. */


    void styleSheetConfiguration();
    void imageConfiguration();
    void iconsConfiguration();
    void buttonSizeConfiguration();

signals:

public slots:
    void changeHeaderPage_InsideExperiment(QStackedWidget *insideExperiment);
    void changePage_InsideExperiment(QStackedWidget *insideExperiment,bool isDensificationPage);


};

#endif // BUTTON_H

/**
 * @file roboruka.h
 *
 * Metody v tomto souboru vám dovolují jednoduše obsluhovat Roboruku.
 *
 */

#ifndef _LIBRB_H
#define _LIBRB_H

#include "rbprotocol.h"

/**
 * \defgroup general Inicializace
 *
 * Tato sekce je určená k počátečnímu nastavení knihovny Roboruka.
 *
 * @{
 */

/**
 * \brief Nastavení SW pro Roboruku
 *
 * Tato struktura obsahuje konfogurační hodnoty pro software Roboruky.
 * Předává se funkci rkSetup(). Ve výchozím stavu má smysluplné hodnoty
 * a není třeba nastavovat všechny, ale jen ty, které chcete změnit.
 */
struct rkConfig {
    rkConfig() :
        rbcontroller_app_enable(false),
        owner(""), name(""),
        battery_coefficient(9.1),
        wifi_name(""), wifi_password(""),
        wifi_default_ap(false), wifi_ap_password("flusflus"), wifi_ap_channel(1),
        motor_id_left(2), motor_id_right(1), motor_max_power_pct(60),
        motor_polarity_switch_left(false), motor_polarity_switch_right(false),
        arm_bone_trims{ 0, 0, 0 }
    {
    }

    bool rbcontroller_app_enable; //!< povolit komunikaci s aplikací RBController. Výchozí: `false`
    rb::Protocol::callback_t rbcontroller_message_callback; //!< funkce, která bude zavolána když přijde zpráva
                                                            //!< z aplikace RBController. Výchozí: žádná


    const char *owner; //!< Jméno vlastníka robota. Podle tohoto jména filtruje RBController roboty. Výchozí: `""`
    const char *name; //!< Jméno robota. Výchozí: ""

    float battery_coefficient; //!< koeficient pro kalibraci měření napětí baterie. Výchozí: `9.1`

    const char *wifi_name; //!< Jméno WiFi sítě, na kterou se připojovat. Výchozí: `""`
    const char *wifi_password; //!< Heslo k WiFi, na kterou se připojit. Výchozí: `""`

    bool wifi_default_ap; //!< Vytvářet WiFi síť místo toho, aby se připojovalo k wifi_name. Výchozí: `false`
    const char *wifi_ap_password; //!< Heslo k vytvořené síti. Výchozí: `"flusflus"`
    uint8_t wifi_ap_channel; //!< Kanál WiFi vytvořené sítě. Výchozí: `1`

    uint8_t motor_id_left; //!< Které M číslo motoru patří levému, podle čísla na desce. Výchozí: `2`
    uint8_t motor_id_right; //!< Které M číslo motoru patří pravému, podle čísla na desce. Výchozí: `1`
    uint8_t motor_max_power_pct; //!< Limit výkonu motoru v procentech od 0 do 100. Výchozí: `60`
    bool motor_polarity_switch_left; //!< Prohození polarity levého motoru. Výchozí: `false`
    bool motor_polarity_switch_right; //!< Prohození polarity pravého motoru. Výchozí: `false`

    float arm_bone_trims[3]; //!< Korekce úhlů pro serva v ruce, ve stupních. Pole je indexované stejně, jako serva,
                             //!< hodnota z tohoto pole je vždy přičtena k úhlu poslenému do serva.
                             //!< Určeno pro korekci nepřesně postavených rukou, kde fyzické postavení ruky
                             //!< neodpovídá vypočítanému postavení.
};

/**
 * \brief Inicializační funkce Roboruky
 *
 * Tuhle funci MUSÍTE zavolat vždy na začátku vaší funkce setup() v main.cpp.
 * Můžete jí předat nastavení ve formě struktury rkConfig.
 */
void rkSetup(const rkConfig& cfg = rkConfig());

/**@}*/
/**
 * \defgroup motors Motory
 *
 * Metody pro obsluhu motorů.
 * @{
 */

/**
 * \brief Nastavení výkonu motorů.
 *
 * \param left výkon levého motoru od od -100 do 100
 * \param right výkon pravého motoru od od -100 do 100
 */
void rkMotorsSetPower(int8_t left, int8_t right);

/**
 * \brief Nastavení motorů podle joysticku.
 *
 * Tato funkce nastaví výkon motorů podle výstupu z joysticku. Očekává dvě
 * hodnoty od -32768 do 32768, posílané například aplikací RBController.
 * Funkce tyto hodnoty převede na výkon a nastaví ho.
 *
 * \param x X hodnota z joysticku.
 * \param y Y hodnota z joysticku.
 */
void rkMotorsJoystick(int32_t x, int32_t y);

/**@}*/
/**
 * \defgroup arm Ruka
 *
 * Metody pro obsluhu samotné ruky.
 * @{
 */

/**
 * \brief Pohnutí rukou na souřadnice X Y.
 *
 * Zadávají se cílové souřadnice pro konec ruky s čelistmi.
 *
 * Souřadnice jsou počítany v milimetrech. Souřadnicový systém má počátek v ose
 * prostředního serva (id 0), doprava od ní se počítá +X, nahoru -Y a dolů +Y.
 * Při pohledu na robota z pravého boku:
 *
 * <pre>
 *                       Y-
 *                       |     /\   <---- Ruka
 *                       |    /  \
 *                       |   /    \
 *                       |  /      \
 *                       | /        C  <-- Cílová pozice, "prsty rukou"
 *                       |/
 * Osa prostředního ---> 0--------------------- X+
 * serva na robotovi.    |
 *                       |
 *                       |
 *                       |
 *                       |
 *                       Y-
 * </pre>
 *
 * Souřadnice nemusíte měřit ručně, jsou vidět v aplikaci RBController nahoře vlevo nad rukou.
 * Vždy je to X a Y nad sebou, zleva první dvojice je pozice kurzoru (červené čárkované kolečko)
 * a druhá je opravdová pozice, na kterou se ruka dostala.
 *
 * Varování: ruka se nedokáže ze všech pozic dostat na všechny ostatní pozice, typicky musí "objet" tělo, aby nenarazila.
 * Pokud pohyb nefunguje, zkuste zadat nějakou pozici, na kterou by se ruka měla dostat odkudkoliv (např. 145;-45)
 * a až pak kýženou cílovou pozici.
 *
 * \param x Cílová souřadnice X
 * \param y Cílová souřadnice Y
 * \return Pokud se podařilo najít řešení a ruka se pohnula, vrátí funkce `true`. Pokud ne, vrátí `false`.
 */
bool rkArmMoveTo(double x, double y);

/**
 * \brief Jsou prsty ruky rozevřené?
 *
 * \return Vrátí `true`, pokud jsou prsty ruky momentálně plně rozevřeny.
 */
bool rkArmIsGrabbing();

/**
 * \brief Otevřít/zavřít prsty ruky.
 *
 * \param grab Pokud `true`, prsty ruky se sevřou, pokud `false` tak se uvolní.
 */
void rkArmSetGrabbing(bool grab);


/**@}*/
/**
 * \defgroup battery Baterie
 *
 * Metody pro získání informací o stavu baterie.
 * @{
 */

/**
 * \brief Úroveň baterie v procentech
 *
 * \return Hodnota od 0 do 100 udávající nabití baterie.
 */
uint32_t rkBatteryPercent();

/**
 * \brief Úroveň baterie v mV.
 *
 * \return Naměřené napětí na baterii v milivoltech.
 */
uint32_t rkBatteryVoltageMv();

uint32_t rkBatteryRaw();

/**@}*/
/**
 * \defgroup rbcontroller Aplikace RBController
 *
 * Metody pro komunikaci s aplikací RBController pro Android.
 * @{
 */

/**
 * \brief Odeslat text do aplikace.
 *
 * Tato metoda odešle text, který se zobrazí v aplikaci v černém okně nahoře.
 * Příklad:
 *
 *     rkControllerSendLog("Test logu! Stav Baterie: %u mv", rkBatteryVoltageMv());
 *
 * \param format Text k odeslání, může obsahovat formátovací značky jako C funkce printf().
 * \param ... argumenty pro format, funguje stejně jako printf().
 */
void rkControllerSendLog(const char *format, ...);

void rkControllerSend(const char *cmd, rbjson::Object *data = nullptr);
void rkControllerSendMustArrive(const char *cmd, rbjson::Object *data = nullptr);

/**@}*/
/**
 * \defgroup leds LEDky
 *
 * Metody pro zapínaní a vypínání LEDek na desce.
 * @{
 */

/**
 * \brief Zapnout/vypnout červenou LED
 * \param on `true` pro zapnuto, `false` pro vypnuto.
 */
void rkLedRed(bool on = true);
/**
 * \brief Zapnout/vypnout žlutou LED
 * \param on `true` pro zapnuto, `false` pro vypnuto.
 */
void rkLedYellow(bool on = true);
/**
 * \brief Zapnout/vypnout zelenou LED
 * \param on `true` pro zapnuto, `false` pro vypnuto.
 */
void rkLedGreen(bool on = true);
/**
 * \brief Zapnout/vypnout modrou LED
 * \param on `true` pro zapnuto, `false` pro vypnuto.
 */
void rkLedBlue(bool on = true);

/**@}*/
/**
 * \defgroup buttons Tlačítka
 *
 * Funkce pro vyčítání stavu tlačítek.
 * @{
 */

/**
 * \brief Je stisknuto tlačítko?
 *
 * Poznámka: tato funkce může trvat až 30ms.
 *
 * \param id číslo tlačítka jako na desce, od 1 do 3 včetně.
 * \return Vrátí `true` pokud je tlačítko stisknuto.
 */
bool rkButtonIsPressed(uint8_t id);

/**@}*/

#endif // LIBRB_H

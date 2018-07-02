using UnityEngine;
using UnityEngine.UI;
using System.Timers;
using System.Collections;

public class mainMenu : MonoBehaviour {

    /**
     * Игровое меню
     */

    /*   Main menu    */
    public Canvas mainCanvas;
    public Button BTContinue;
    public GameObject alpha;

    /*   Options menu    */

    public Canvas optionCanvas;
    public Slider musicSlider;
    public Slider soundSlider;

    /*  Confirm Menu  */

    public Canvas confirmCanvas;

    /*  Animations   */

    private Animator _menuAnim;
    private Animator _optionsAnim;
    private Animator _confirmAnim;

    private float _temp;
    private Timer _timer;
    private bool _close = false;
    private bool _start = false;

    double deltaTime;
    double fps;

    void Start ()
    {
        Init();
    }
    private void Init()
    {
        disableOptionsMenu();

        /*  Initialize anims  */
        _menuAnim = mainCanvas.GetComponent<Animator>();
        _optionsAnim = optionCanvas.GetComponent<Animator>();
        _confirmAnim = confirmCanvas.GetComponent<Animator>();

        _timer = new Timer();
        _menuAnim.CrossFade("menuOpen", 1.0f);

        // Check savefiles in memory
        if (PlayerPrefs.HasKey("hasSaveFiles") &&
            PlayerPrefs.GetString("hasSaveFiles") != "false")
        {
            BTContinue.GetComponent<Button>().interactable = true;
        }
        else { BTContinue.GetComponent<Button>().interactable = false; }
    }

    #region mainMenu Buttons
    /*   mainMenu buttons Action   */
    public void BTContinuePressed()
    {
        PlayerPrefs.SetString("loadGame", "true");
        _menuAnim.CrossFade("menuClose", 1.0f);
        initFader("start");
    }
    public void BTNewHistoryPressed()
    {
        if (PlayerPrefs.GetString("hasSaveFiles") == "true")
        {
            _confirmAnim.CrossFade("confirmOpen", 1.0f);
        } else
        {
            PlayerPrefs.SetString("hasSaveFiles", "false");
            PlayerPrefs.SetString("loadGame", "false");
            _menuAnim.CrossFade("menuClose", 1.0f);
            initFader("start");
        }
    }
    public void BTOptionsPressed()
    {
        if (optionCanvas.enabled == false)
        {
            _menuAnim.CrossFade("menuClose", 1.0f);
            _optionsAnim.CrossFade("optionsOpen", 1.0f);
        }     
    }
    public void BTQuitPressed()
    {
        initFader("quit");
        _menuAnim.CrossFade("menuClose", 1.0f);
    }
    // Quit after full black sreen
    IEnumerator appEnum()  
    {
        while (true)
        {
            if (_close == true)
            {
                Application.Quit();
                break;
            }
            else if(_start == true)
            {
                Application.LoadLevel(1);
            }
            yield return new WaitForSeconds(1);
        }
    }
    // Action after timer elapsed
    private void Timer_Elapsed_exit(object sender, ElapsedEventArgs e)
    {
        _close = true;
        _timer.Stop();
    }
    private void Timer_Elapsed_start(object sender, ElapsedEventArgs e)
    {
        _start = true;
        _timer.Stop();
    }

    public void BTNoConfirmPressed()
    {
        _confirmAnim.CrossFade("confirmClose", 1.0f);
    }
    public void BTYesConfirmPressed()
    {
        PlayerPrefs.SetString("hasSaveFiles", "false");
        PlayerPrefs.SetString("loadGame", "false");
        _confirmAnim.CrossFade("confirmClose", 1.0f);
        _menuAnim.CrossFade("menuClose", 1.0f);
        initFader("start");
    }
    private void initFader(string state)
    {
        
        alpha.GetComponent<ScreenFader>().fadeState = ScreenFader.FadeState.In; // Fade screen
        _timer.Interval = 2000;

        if(state == "start") { _timer.Elapsed += Timer_Elapsed_start; }
        else if(state == "quit") { _timer.Elapsed += Timer_Elapsed_exit; }
        
        _timer.Start(); // Start timer for 2 seconds
        StartCoroutine(appEnum());
    }
    #endregion mainMenu Buttons

    #region optionMenu Buttons
    /*   optionMenu buttons Action   */

    public void BTSaveOptionsPressed()
    {
        PlayerPrefs.SetFloat("musicVolume", musicSlider.value / 10);
        PlayerPrefs.SetFloat("soundVolume", soundSlider.value / 10);
        GetComponent<refreshSettigs>().refreshSettings();
    }
    public void BTBackToMenuPressed()
    {
        _menuAnim.CrossFade("menuOpen", 1.0f);
        _optionsAnim.CrossFade("optionsClose", 1.0f);
    }
    #endregion

    #region Enable&Disable
    void enableMainMenu()
    {
        mainCanvas.enabled = true;
    }
    void enableOptionsMenu()
    {
        optionCanvas.enabled = true;
    }
    void disableMainMenu()
    {
        mainCanvas.enabled = false;
    }
    void disableOptionsMenu()
    {
        optionCanvas.enabled = false;
    }
    #endregion Enable&Disable
}

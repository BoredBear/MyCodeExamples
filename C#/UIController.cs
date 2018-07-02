using UnityEngine;
using TMPro;
using System.Collections;
using System.Collections.Generic;

public class UIController : MonoBehaviour {

    [SerializeField] private GameObject btGrabItem;
    [SerializeField] private GameObject JoyBackground;
    [SerializeField] private GameObject JoyForeground;
    [SerializeField] private GameObject InfoTextObj;
    private TextMeshProUGUI tmpInfoText;
    private List<string> messageList;
    private float dilate = -1;
    
    private void Start()
    {
        tmpInfoText = InfoTextObj.GetComponent<TextMeshProUGUI>();
        messageList = new List<string>();
    }

    #region InfoText
    /// <summary>
    /// Выводит нужный текст на экран
    /// </summary>
    /// <param name="message"></param>
    public void ShowInfoText(string message)
    {
        if (messageList.Count < 1)
        {
            dilate = -1;
            tmpInfoText.fontMaterial.SetFloat(ShaderUtilities.ID_FaceDilate, dilate);
            tmpInfoText.SetText(message);
            InfoTextObj.SetActive(true);
            StartCoroutine(InfoTextAnimationShow());
            messageList.Add(message);
        }
        else
        {
            messageList.Add(message);
        }
    }

    public void ShowLatelyMessage()
    {
        if (messageList.Count > 0)
        {
            string message = messageList[0];
            tmpInfoText.fontMaterial.SetFloat(ShaderUtilities.ID_FaceDilate, dilate);
            tmpInfoText.SetText(message);
            InfoTextObj.SetActive(true);
            StartCoroutine(InfoTextAnimationShow());           
        }
        else
        {
            InfoTextObj.SetActive(false);
        }
    }

    private void UpdateDilateShow()
    {
        if (dilate < -0.1f)
        {
            tmpInfoText.fontMaterial.SetFloat(ShaderUtilities.ID_FaceDilate, dilate);
            dilate += 0.1f;
            StartCoroutine(InfoTextAnimationShow());
        } else
        {
            dilate = -0.1f;
            StartCoroutine(InfoTextDelay());
        }
    }

    private void UpdateDilateHide()
    {
        if(dilate > -1)
        {
            dilate -= 0.1f;
            tmpInfoText.fontMaterial.SetFloat(ShaderUtilities.ID_FaceDilate, dilate);            
            StartCoroutine(InfoTextAnimationHide());
        } else
        {
            HideInfoText();
        }
    }

    public void HideInfoText()
    {
        dilate = -1;
        tmpInfoText.fontMaterial.SetFloat(ShaderUtilities.ID_FaceDilate, dilate);
        messageList.Remove(tmpInfoText.text);
        ShowLatelyMessage();
    }

    IEnumerator InfoTextAnimationShow()
    {
        yield return new WaitForSeconds(0.05f);
        UpdateDilateShow();
    }

    IEnumerator InfoTextAnimationHide()
    {
        yield return new WaitForSeconds(0.05f);
        UpdateDilateHide();
    }

    IEnumerator InfoTextDelay()
    {
        // Тут нужно сделать, чтобы длина задержки зависела от количества символов
        yield return new WaitForSeconds(2f);
        UpdateDilateHide();
    }
    #endregion

    #region ButtonGrabItem
    public void ShowBtGrabItem()
    {
        btGrabItem.SetActive(true);
    }

    public void HideBtGrabItem()
    {
        btGrabItem.SetActive(false);
    }
    #endregion

    #region Joystick

    public void SetPositionJoyBackground(Vector3 pos)
    {
        JoyBackground.transform.position = pos;
    }
    
    public void SetPositionJoyForeground(Vector3 pos)
    {
        JoyForeground.transform.localPosition = pos;
    }

    public void ShowJoy()
    {
        JoyBackground.SetActive(true);
    }

    public void HideJoy()
    {
        JoyBackground.SetActive(false);
    }

    public Vector3 GetJoyBackgroundPos()
    {
        return JoyBackground.transform.position;
    }
    #endregion

    public void HideUI()
    {
        gameObject.SetActive(false);
    }
}


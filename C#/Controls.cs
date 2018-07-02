using UnityEngine;

[RequireComponent(typeof(PlayerController))]
public class Controls : MonoBehaviour {

    [Range(0.5f, 3.0f)] public float InteractiveDistance = 1.0f;
    private UIController uiController;
    private Shader interactShader;
    private Shader standartShader;
    private Collider lastHittedObj;
    private bool isChangedShade = false;
    private PlayerController _controller;
    private bool _isPaused;
    private Vector3 _anchor; // Joystick Background Center 
    private Vector3 _lastRotationTouch;
    private Vector3 _moveVector;
    private float _joystickRadius;
    private bool _anchorSet = false; // True если джойстик стоит
    private int _indexOfJoystickTouch = -1; // -1 если джойстик не установлен на экране
    private bool _isJoystickPressed = false;
    private bool isObjectDragged = false;
    int layerM;

    private void Start()
    {
        uiController = GameObject.Find("UI").GetComponent<UIController>();
        _controller = GetComponent<PlayerController>();
        layerM = LayerMask.GetMask("Interactive");
        interactShader = Shader.Find("graphs/StandartInt_Shader");
        standartShader = Shader.Find("graphs/Standart_Shader");

#if UNITY_EDITOR
        //uiController.HideUI();
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
#else
        _joystickRadius = uiController.GetJoyBackgroundPos().x;
        _moveVector = Vector3.zero;
#endif     
    }

    public void FixedUpdate()
    {
#if UNITY_EDITOR
        DebugControl();
#else
        TouchControl();
#endif
        ExecuteRaycast();
    }
#if UNITY_EDITOR
    private void Update()
    {
        if (!_isPaused)
        {
            if (Input.GetMouseButtonDown(0))
            {
                InterractWithObj(0);
            }
            else if (Input.GetMouseButtonDown(1)) 
            {
                if (isObjectDragged)
                {
                    InterractWithObj(2);
                }
                else
                {
                    InterractWithObj(1);
                }
            } else if (isObjectDragged)
            {
                UpdateScroll();
            }
        }
    }
#endif

#if UNITY_EDITOR
    private void DebugControl()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            ButtonPausePressed();
        }
        else if (!_isPaused)
        {
            var vector = new Vector3(Input.GetAxis("axisX"), 0, Input.GetAxis("axisY"));
            _controller.Move(vector.normalized);
            vector = new Vector2(Input.GetAxis("MouseX"), Input.GetAxis("MouseY"));
            _controller.Rotate(vector);
        }
    }
#else
    private void TouchControl()
    {
        // Проверка тача и считывание показаний джойстика
        if ((Input.touchCount > 0 || Input.GetMouseButton(0)) && enabled)
        {
            int count = Input.touchCount;
            for (int i = 0; i < count; i++)
            {
                ProcessingTouches(Input.GetTouch(i));
            }
            // Для дебага из редактора
            _controller.Move(_moveVector);
            if (!_isJoystickPressed)
            {
                HideJoystick();
            }
            _isJoystickPressed = false;
        }
        else
        {
            HideJoystick();
        }
    }

    // Обработка нажатий на экран
    private void ProcessingTouches(Touch touch)
    {
        Vector3 mousePos = touch.position;
        // Если это джойстик
        if (touch.fingerId == _indexOfJoystickTouch || _indexOfJoystickTouch == -1)
        {
            if (isJoystickPosition(mousePos))
            {
                _isJoystickPressed = true;
                if (_anchorSet)
                {
                    Vector3 newPos = mousePos - _anchor;
                    if (newPos.magnitude > _joystickRadius)
                    {
                        newPos.Normalize();
                        uiController.SetPositionJoyForeground(newPos * _joystickRadius);
                        newPos.z = newPos.y;
                        newPos.y = 0;
                        _moveVector = newPos.normalized;
                    }
                    else
                    {
                        uiController.SetPositionJoyForeground(newPos);
                        newPos.z = newPos.y;
                        newPos.y = 0;
                        _moveVector = newPos / _joystickRadius;
                    }
                } else
                {
                    if (mousePos.x < _joystickRadius) { mousePos.x = _joystickRadius; }
                    if (mousePos.y < _joystickRadius) { mousePos.y = _joystickRadius; }
                    if (mousePos.x > Screen.width * 0.4f - _joystickRadius) { mousePos.x = Screen.width * 0.4f - _joystickRadius; }
                    if (mousePos.y > Screen.height * 0.6f - _joystickRadius) { mousePos.y = Screen.height * 0.6f - _joystickRadius; }
                    uiController.SetPositionJoyBackground(mousePos);
                    uiController.ShowJoy();
                    _anchor = mousePos;
                    _indexOfJoystickTouch = touch.fingerId;
                    _anchorSet = true;
                }
            }
        }
        // Если это поворот
        if (isRotationPosition(mousePos) && touch.phase == TouchPhase.Moved)
        { 
            Vector2 delta = touch.deltaPosition;
            float rotationZ = delta.x * 2 * Time.deltaTime;
            float rotationX = delta.y * 2 * Time.deltaTime * -1; // -1 для инвертирования обзора
            _controller.Rotate(new Vector2(rotationX, rotationZ)); 
        }

        if(isActionPosition(mousePos))
        {
            if(isObjectDragged)
            {
                _controller.ZoomDragObject(touch.deltaPosition.y);
            }
            else if(lastHittedObj != null)
            {
                InterractWithObj(0);
            } 
        }
    }
    private bool isActionPosition(Vector3 mousePos)
    {
        return mousePos.x > Screen.width * 0.45 &&
               mousePos.x < Screen.width * 0.55 &&
               mousePos.y > Screen.height * 0.3;
    }

    private bool isRotationPosition(Vector3 mousePos)
    {
        return mousePos.x > Screen.width * 0.55;
    }

    private bool isJoystickPosition(Vector3 mousePos)
    {
        return mousePos.x < Screen.width * 0.45 &&
               mousePos.y < Screen.height * 0.7;
    }

    private void HideJoystick()
    {
        _moveVector = Vector3.zero;
        uiController.HideJoy();
        _anchorSet = false;
        _indexOfJoystickTouch = -1;
    }
#endif

    private void ExecuteRaycast()
    {
        RaycastHit hit;
        // Используем 8 слой - интерактивный 
        if (Physics.Raycast(_controller.PlayerCamera.transform.position,
                            _controller.PlayerCamera.transform.forward,
                            out hit,
                            InteractiveDistance, layerM))
        {
            if (isChangedShade == false)
            {      
                isChangedShade = true;
                hit.collider.GetComponent<Renderer>().material.shader = interactShader;
                lastHittedObj = hit.collider;
                uiController.ShowBtGrabItem();
            }
        } else if (lastHittedObj != null)
        {
            lastHittedObj.GetComponent<Renderer>().material.shader = standartShader;
            lastHittedObj = null;
            isChangedShade = false;
            uiController.HideBtGrabItem();
        }
    }

    public void ButtonGrabPressed()
    {
        if (isObjectDragged)
        {
            InterractWithObj(2);
        }
        else
        {
            InterractWithObj(1);
        }
    }

    private void InterractWithObj(byte code)
    {
        if (lastHittedObj != null)
        {
            switch (code)
            {
                case 0:
                    _controller.InterractWithObject(lastHittedObj.gameObject);
                    break;
                case 1:
                    isObjectDragged = _controller.SetDragObject(lastHittedObj.gameObject);
                    break;
                case 2:
                    isObjectDragged = _controller.PutDownDragObject();
                    break;
            }
        }
    }
#if UNITY_EDITOR
    private void UpdateScroll()
    {
        _controller.ZoomDragObject(Input.mouseScrollDelta.y);
    }
#endif

    public void ButtonPausePressed()
    {
        //_isPaused = true;
        //Time.timeScale = 0f;
        // Старт анимации паузы
    }
}

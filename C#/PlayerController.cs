using System.Collections;
using UnityEngine;

/// <summary>
/// Скрипт используется для управления игроком
/// </summary>
[RequireComponent(typeof(Controls))]
public class PlayerController : MonoBehaviour {

    #region Parameters
    [SerializeField, Range(0.5f, 100f)] private float playerSpeedRotation = 1.0f;
    [SerializeField, Range(0, 1f)] private float moveSmoothness = 0.1f;
    [SerializeField, Range(0, 1f)] private float rotateSmoothness = 0.1f;
    [SerializeField, Range(1f, 2f)] private float maxDragDistance;
    [SerializeField, Range(0.5f, 1f)] private float minDragDistance;
    private InventoryController invController;
    private CharacterController controller;
    private UIController uiController;
    public Camera PlayerCamera;
    private GameObject dragObject;
    private float gravityModifier = 9.8f;
    private float speedChangeParam = 0.01f;
    private float maxFallSpeed = 20f;
    private float currentSpeed = 0f;
    private float hbX, hbY; // HeadBob
    private float deltaHbX = 0;
    private float deltaHbY = 0;
    private float playerHeight;
    private bool hbEnded = true;
    private bool interactiveDelay = false;
    public int PlayerHealth { get; private set; }
    private Vector3 lastPos;
    private Vector3 desiredPos;
    private Vector2 desiredRot;
    private float DragDistance;
    

    #endregion

    private void Start()
    {
        controller = GetComponent<CharacterController>();
        invController = GetComponent<InventoryController>();
        uiController = GameObject.Find("UI").GetComponent<UIController>();
        desiredRot = PlayerCamera.transform.localEulerAngles;
        playerHeight = controller.height / 2;
    }

    private void FixedUpdate()
    {
        PhysicsUpdate();
        MoveUpdate();
        RotateUpdate();
        DragObject();
        if(hbEnded)
        {
            StartCoroutine(HeadBobUpdate());
            hbEnded = false;
        }
    }

    private void PhysicsUpdate()
    {
        if (!controller.isGrounded)
        {
            // Летим вверх
            if (transform.position.y > lastPos.y)
            {
                currentSpeed -= gravityModifier * speedChangeParam;
            }
            else
            {
                if (currentSpeed < maxFallSpeed)
                {
                    currentSpeed += gravityModifier * speedChangeParam;
                }
            }
            controller.Move(Vector3.down * Time.deltaTime * currentSpeed);
            lastPos = transform.position;
        }
        else
        {
            currentSpeed = 0f;
        }
    }

    #region WorkWithObjects
    // Взаимодействие с объектом, если произведено нажатие
    public void InterractWithObject(GameObject obj)
    {       
        InteractiveObject intObj = obj.GetComponent<InteractiveObject>();
        if (intObj == null || interactiveDelay) { return; }
        interactiveDelay = true;
        StartCoroutine(InteractiveDelay());
        switch (intObj.InteractiveTag)
        {
            case Interactive.InteractiveTag.Action:
                /*
                 * Вызываем метод действия, скрипт сам решит, что нужно выполнить
                 */
                break;
            case Interactive.InteractiveTag.Animation:
                /*
                 * Если это объект анимации, производим 
                 * вызов воспроизведения анимации
                 */
                intObj.PlayAnimation();
                break;
            case Interactive.InteractiveTag.Pick:
                if(invController.PickObject(obj))
                {
                    
                    uiController.ShowInfoText("You picked the " + obj.name);
                    uiController.ShowInfoText("CurrentWeight " + invController.GetCurrentWeight() + "\nMaxWeight " + invController.GetMaxWeight());
                }
                else
                {
                    uiController.ShowInfoText("You don't have free space");
                }
                break;
        }
    }

    IEnumerator InteractiveDelay()
    {
        yield return new WaitForSeconds(0.5f);
        interactiveDelay = false;
    }

    private void DragObject()
    {
        if (dragObject != null)
        {
            dragObject.transform.position = Vector3.Lerp(dragObject.transform.position, PlayerCamera.transform.position + (PlayerCamera.transform.forward * DragDistance), 0.2f);
            CorrectDragObjPosition();   
        }
    }

    /// <summary>
    /// Используется для запрета игроку
    /// стоять на перетаскиваемом объекте
    /// </summary>
    private void CorrectDragObjPosition()
    {
        if (dragObject.transform.localPosition.z < controller.radius)
        {
            if (dragObject.transform.localPosition.z > 0)
            {
                dragObject.transform.localPosition = new Vector3(dragObject.transform.localPosition.x, dragObject.transform.localPosition.y, controller.radius * 1.01f);
            }
            else if (dragObject.transform.localPosition.z > -controller.radius)
            {
                dragObject.transform.localPosition = new Vector3(dragObject.transform.localPosition.x, dragObject.transform.localPosition.y, -controller.radius * 1.01f);
            }
        }

    }

    public bool SetDragObject(GameObject dragObj)
    {
        Rigidbody rb = dragObj.GetComponent<Rigidbody>();
        rb.useGravity = false;
        rb.freezeRotation = true;
        DragDistance = 0.8f;
        dragObject = dragObj;
        dragObject.transform.SetParent(transform);
        return true;
    }
    
    public bool PutDownDragObject()
    {
        Rigidbody rb = dragObject.GetComponent<Rigidbody>();
        rb.useGravity = true;
        rb.freezeRotation = false;
        dragObject.transform.parent = null;
        dragObject = null;
        return false;
    }

    public void ZoomDragObject(float delta)
    {
        DragDistance += delta * 0.05f;
        if (DragDistance > maxDragDistance) { DragDistance = maxDragDistance; }
        else if(DragDistance < minDragDistance) { DragDistance = minDragDistance; }             
    }

    #endregion

    #region Moving and Rotation

    /// <summary>
    /// Сглаженное перемещение игрока к данной позиции
    /// </summary>
    private void MoveUpdate()
    {
        if (controller.isGrounded)
        {
            Vector3 newV = Vector3.Lerp(transform.position, desiredPos, moveSmoothness);
            controller.Move(newV - transform.position);
        }
    }

    private void RotateUpdate()
    {
        float x = Mathf.LerpAngle(PlayerCamera.transform.localEulerAngles.x, desiredRot.x, rotateSmoothness);
        float y = Mathf.LerpAngle(transform.localEulerAngles.y, desiredRot.y, rotateSmoothness);
        PlayerCamera.transform.localEulerAngles = new Vector3(x, PlayerCamera.transform.localEulerAngles.y, PlayerCamera.transform.localEulerAngles.z);
        transform.localEulerAngles = new Vector3(transform.localEulerAngles.x, y, transform.localEulerAngles.z);
    }
    
    /// <summary>
    /// Обновление целевой позиции
    /// </summary>
    /// <param name="moveVector"></param>
    public void Move(Vector3 moveVector)
    {
        if (Vector3.Distance(transform.position, desiredPos) > 0.15f)
        {
            desiredPos -= (desiredPos - transform.position) / 2;
        }
        moveVector = Quaternion.Euler(0, transform.rotation.eulerAngles.y, 0)
                 * moveVector
                 * Time.deltaTime;
        desiredPos += moveVector;
        desiredPos.y = transform.position.y * 0.98f; // Поправка для прижатия к поверхности
        GameObject.FindGameObjectWithTag("DEBUGCUBE").transform.position = desiredPos;
    }

    /// <summary>
    /// Поворот игрока
    /// </summary>
    /// <param name="currentRotateVector"></param>
    public void Rotate(Vector2 angle)
    {
        angle *= playerSpeedRotation; 
        desiredRot += angle;
        if (desiredRot.x < -80) desiredRot.x = -80; // 80 - Максимальный градус поднятия головы
        if (desiredRot.x > 80) desiredRot.x = 80;
        if (desiredRot.y < -360f) desiredRot.y += 720f;
        if (desiredRot.y > 360f) desiredRot.y -= 720f;   
    }
    #endregion

    #region PlayerLogic
    /// <summary>
    /// Повреждение игрока, вызывается противниками, огнём, etc
    /// </summary>
    /// <param name="damage"></param>
    public void Hurt(int damage)
    {
        PlayerHealth -= damage;
        if(PlayerHealth < 1)
        {
            Debug.Log("Player Died");
        }
    }
    #endregion

    #region HeadBob
    IEnumerator HeadBobUpdate()
    {
        hbX = Random.Range(-0.1f, 0.1f);
        hbY = Random.Range(-0.1f, 0.1f);
        deltaHbX += hbX;
        deltaHbY += hbY;
        if(deltaHbX > 1 || deltaHbX < -1)
        {
            deltaHbX -= deltaHbX;
            hbX = -hbX;
        } 
        if(deltaHbY > 1 || deltaHbY < -1)
        {
            deltaHbY -= deltaHbY;
            hbY = -hbY;
        }
        Rotate(new Vector2(hbX, hbY));
        yield return new WaitForSeconds(0.15f);
        hbEnded = true;
    }
    #endregion
}

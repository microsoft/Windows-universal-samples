using UnityEngine;
using System.Collections;

public class Turn : MonoBehaviour {
    public float turningSpeed = 60;

    // Use this for initialization
    void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
        float horizontal = Input.GetAxis("Horizontal") * turningSpeed * Time.deltaTime;
        transform.Rotate(0, horizontal, 0);
    }
}

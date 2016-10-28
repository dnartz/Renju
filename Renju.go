// +build !windows
package main

// #cgo LDFLAGS: -L ./  -lRenju -lstdc++
// #include "chess.h"
import "C"

import (
	"fmt"
	"log"
	"time"
	"net/http"
	"io/ioutil"
	"encoding/json"
)

type player struct{
	Player_type string `json:"type"`
	Name string `json:"name"`
	Url string `json:"url"`
	Side string `json:"side"`
}

type action struct {
	Side string `json:"side"`
	X int `json:"x"`
	Y int `json:"y"`
	Time string `json:"time"`
}

type message struct {
	Head struct {
		Msg_type int `json:"type"`
		Result int `json:"result"`
		Err_msg string `json:"err_msg"`
	} `json:"head"`
	Body struct {
		Player_white *player `json:"player_white"`
		Player_black *player `json:"player_black"`
		Start_time string `json:"start_time"`
		Size int `json:"size"`
		Steps []action `json:"steps"`
		Id string `json:"id"`
		Has_hand_cut int `json:"has_hand_cut"`
		Winner string `json:"winner"`
	} `json:"body"`
}

// 谁先手：1为我方（我方执黑），0为对手（我方执白）
var whoseTurn int = -1

var msgTemplate message

func setPlayerInfo(msg message) (string, error) {
	if msg.Body.Player_white != nil {
		whoseTurn = 0
		(*msg.Body.Player_white).Name = "探索者队"
	} else {
		whoseTurn = 1
		(*msg.Body.Player_black).Name = "探索者队"
	}

	if result, err := json.Marshal(msg); err == nil {
		msgTemplate = msg
		C.initMap(C.int(whoseTurn))

		return string(result), nil
	} else {
		log.Fatal(err)
		return "", err
	}
}

func makeMove(msg message) message {
	var newStep action
	newMsg := msgTemplate

	// 如果是我们下第一步，在调用了C++的initMap之后就已经下完第一步（天元）了。
	// 在这里我们直接返回
	if msg.Body.Steps == nil || len(msg.Body.Steps) == 0 {
		newStep.Side = "b"
		newStep.Time = time.Now().Format("20160417161058")
		newStep.X = 8
		newStep.Y = 8

		newMsg.Body.Steps = []action{newStep}

		return newMsg;
	}

	lastStep := msg.Body.Steps[len(msg.Body.Steps) - 1]
	C.playGame(C.int(lastStep.Y - 1), C.int(lastStep.X - 1))

	newStep.Time = time.Now().Format("20160417161058")
	if whoseTurn == 0 {
		newStep.Side = "w"
	} else {
		newStep.Side = "b"
	}
	newStep.X = int(C.lastStep[1]) + 1
	newStep.Y = int(C.lastStep[0]) + 1
	newMsg.Body.Steps = []action{newStep}

	return newMsg
}

func handler(w http.ResponseWriter, r *http.Request) {
	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Fatal(err)
		return
	}
	fmt.Printf("%s\n", body)

	var msg message

	err = json.Unmarshal(body, &msg)
	if err != nil {
		log.Fatal(err)
		return
	}

	var res string

	if (whoseTurn == -1) {
		res, err = setPlayerInfo(msg)
	} else {
		msg = makeMove(msg)
		var buf []byte
		buf, err = json.Marshal(msg)

		res = string(buf)
	}

	if err != nil {
		log.Fatal(err)
	} else {
		fmt.Printf("%s\n", res)
		fmt.Fprintf(w, "%s", res)

		// 如果得出胜负，比赛已分，那么恢复到初始状态
		if (C.win != -1) {
			whoseTurn = -1
		}
	}
}

func main()  {
	http.HandleFunc("/", handler)
	log.Fatal(http.ListenAndServe(":80", nil))

	var i, j int

	for (C.win == 0) {
		fmt.Scanf("%d %d", &i, &j)

		C.playGame(C.int(i), C.int(j))
	}
}
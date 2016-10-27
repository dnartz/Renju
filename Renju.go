// +build !windows
package main

// #cgo LDFLAGS: -L ./  -lRenju -lstdc++
// #include "chess.h"
import "C"

import (
	"fmt"
	"log"
	"net/http"
	"io/ioutil"
	"encoding/json"
)

func handler(w http.ResponseWriter, r *http.Request) {
	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Fatal(err)
		return
	}

	var cmd map[string]*json.RawMessage

	json.Unmarshal(body, &cmd)
	json.Unmarshal(*cmd["body"], cmd)

	var str string
	json.Unmarshal(*cmd["url"], &str)
	fmt.Printf("%s", str)

	fmt.Fprintf(w, "test")
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
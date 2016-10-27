package Renju

// +build !windows
// #cgo LDFLAGS: -L ./  -lRenju -lstdc++
// #include "chess.h"
import "C"

import (
	"fmt"
)

func main()  {
	var i, j int

	for (C.win == 0) {
		fmt.Scanf("%d %d", &i, &j)

		C.playGame(C.int(i), C.int(j))
	}
}

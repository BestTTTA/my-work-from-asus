package main

import "fmt"

type Player struct {
	Username string
	Level    uint
	Status   string
	Class    string
}

func (player *Player) Uplevel() {
	player.Level++
	// fmt.Println(player.Level)
}

func main() {

	p := Player{
		Username: "best",
		Level:    0,
		Status:   "on",
		Class:    "mage",
	}
	// fmt.Println(p.Level)

	for range 5 {
		p.Uplevel()
	}

	fmt.Println(p.Level)
}

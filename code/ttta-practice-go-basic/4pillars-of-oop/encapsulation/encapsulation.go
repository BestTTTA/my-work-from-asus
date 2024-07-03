package main

import "fmt"

type (
	PlayerClass interface {
		Attack()
	}

	Warrior struct {
		Weapon string
	}
	Mage struct {
		Spell string
	}
)

func (w Warrior) Attack() {
	fmt.Println("Warrior attack by", w.Weapon)
}
func (m Mage) Attack() {
	fmt.Println("Mage attack by", m.Spell)
}

func PlayerAttack(p PlayerClass){
	p.Attack()
}

func main() {

	w := Warrior{
		Weapon: "Sword",
	}
	m := Mage{
		Spell: "Fireball",
	}

	// w.Attack()
	// m.Attack()
	PlayerAttack(w)
	PlayerAttack(m)

}
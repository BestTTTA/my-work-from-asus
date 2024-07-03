package main

import "fmt"

type (
	Human struct{}
	Devil struct {
		Human
	}
	Elf struct {
		Human
	}
)

func (h Human) Walk() {
	fmt.Println("walk")
}
func (h Human) Breath() {
	fmt.Println("Breath")
}
func (h Human) Eat() {
	fmt.Println("Eat")
}

func (d Devil) Mutate() {
	fmt.Println("mutate")
}
func (d Devil) Fly() {
	fmt.Println("fly")
}

func (e Elf) MagicSpell() {
	fmt.Println("cast magic")
}

func main() {
	human := Human{}
	devil := Devil{}
	elf := Elf{}

	fmt.Println("Human")
	human.Walk()
	human.Eat()
	human.Breath()

	fmt.Println()

	fmt.Println("Devil")
	devil.Walk()
	devil.Eat()
	devil.Breath()
	devil.Mutate()
	devil.Fly()

	fmt.Println()

	fmt.Println("Elf")
	elf.Walk()
	elf.Eat()
	elf.Breath()
	elf.MagicSpell()



}

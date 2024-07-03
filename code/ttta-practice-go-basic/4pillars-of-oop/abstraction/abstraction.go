package main

import "fmt"

type (
	Keyboard interface {
		Input()
	}

	MechanicalKeyboard struct {
		SwitchType string
		Size       string
		OS         string
	}

	NormalKeyboard struct {
		CanPress bool
	}
)

func (m *MechanicalKeyboard) Input() {
	fmt.Println("MechanicalKeyboard have func:", m.SwitchType, m.Size, m.OS)
}
func (m *NormalKeyboard) Input() {
	fmt.Println("NormalKeyboard have func:", m.CanPress)
}

func main() {

	mec := MechanicalKeyboard{
		SwitchType: "some-type",
		Size: "md",
		OS: "mac",
	}
	nor := NormalKeyboard{
		CanPress: true,
	}

	mec.Input()
	nor.Input()

}

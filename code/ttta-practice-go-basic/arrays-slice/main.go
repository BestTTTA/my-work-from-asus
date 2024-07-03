package main

import "fmt"

func main() {
	// var a [5]int = [5]int{1, 2, 3, 4, 5}
	// var b [3]string = [3]string{"best1", "best2", "best3"}
	// fmt.Println(a)
	// fmt.Println(b)

	// for i := range a {
	// 	fmt.Println(i)
	// }
	// fmt.Println(b[2])

	// for i := range b {
	// 	fmt.Println(b[i])
	// }

	// b := []string{"best1", "best2", "best3"}
	// b = append(b, "best4")
	// fmt.Println(b)

	// for i := range b {
	// 	fmt.Println(b[i])
	// }


	c := []int{1,2,3}
	c = append(c, 4)
	address(c)


}

func address(n []int){
	for i := range n {
		fmt.Printf("address slice c %v\n", &n[i])
	}
}
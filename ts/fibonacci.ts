class Fibonacci {
	
	private static ComputeFib(index: number): number {
		if (index == 0 || index == 1)
			return 1;
		return Fibonacci.ComputeFib(index - 1) + 
			   Fibonacci.ComputeFib(index - 2);
	}
	
	public static main(): void {
		var value: number = Fibonacci.ComputeFib(5);
	}
}
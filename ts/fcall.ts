class A {
    public x(y: number, z: string):number {
        return A.x(y - 1, z[0]++);
    }
}
function Start()
    print("Start")
end

function Update(dt)
    print("Test", 25)
    Transform.translation.x = Transform.translation.x + 10
    print("Update")
end

function Shutdown()
    print("Shutdown")
end
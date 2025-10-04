function Start()
	print("Start Emitting")
end

function Update(dt)
	if Emitter ~= nil and Transform ~= nil then
		Emitter:Emit(Transform)
	else
		error("Error")
	end
end

function Shutdown()
	print("Shutdown")
end
		
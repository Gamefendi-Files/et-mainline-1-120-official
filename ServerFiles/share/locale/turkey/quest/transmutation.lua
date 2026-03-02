quest transmutation begin
	state start begin
		when 60003.chat."Transmutation" begin
			say("Transmutation enables you to permanently change[ENTER]the appearance of your equipment. The effect is[ENTER]restricted to the level of your equipment,[ENTER]however, and the transmutation will be reversed[ENTER]if you improve the item. Do you want to continue?")
			say("")
			local s = select(gameforge.locale.yes, gameforge.locale.no)
			if s == 1 then
				setskin(NOWINDOW)
				game.open_transmutation(true)
			end
		end
		
		when 60003.chat."Mount Transmutation" begin
			setskin(NOWINDOW)
			game.open_transmutation(false)
		end
	end
end
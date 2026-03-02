quest transmutation begin
	state start begin
		when 60003.chat."Kostüm-Zýrh-Silah-Baþlýk Transfer" begin
				setskin(NOWINDOW)
				game.open_transmutation(true)
		end
		
		when 60003.chat."Binek Transfer" begin
			setskin(NOWINDOW)
			game.open_transmutation(false)
		end
	end
end
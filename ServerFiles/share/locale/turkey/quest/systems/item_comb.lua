quest item_comb begin
	state start begin
		when 60003.chat."Kostüm Bonus Aktarma" begin
				setskin(NOWINDOW)
				game.open_item_comb()
		end
	end
end